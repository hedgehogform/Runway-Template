#include "Toolkit/AssetGeneration/AssetDumpViewWidget.h"
#include "PackageTools.h"
#include "Toolkit/AssetGeneration/AssetTypeGenerator.h"

#define LOCTEXT_NAMESPACE "AssetGenerator"

FString CollectQuotedString(const FString &SourceString, const int32 StartOffset)
{
	FString ResultString;
	bool bLastSymbolBackslash = false;

	for (int32 i = StartOffset; i < SourceString.Len(); i++)
	{
		const TCHAR CharAtIndex = SourceString[i];
		if (bLastSymbolBackslash)
		{
			ResultString.AppendChar(CharAtIndex);
			bLastSymbolBackslash = false;
		}
		else if (CharAtIndex == '\\')
		{
			bLastSymbolBackslash = true;
		}
		else if (CharAtIndex != '"')
		{
			ResultString.AppendChar(CharAtIndex);
		}
		else
		{
			break;
		}
	}
	return ResultString;
}

FAssetDumpTreeNode::FAssetDumpTreeNode()
{
	this->bIsLeafNode = false;
	this->bIsChecked = false;
	this->bIsOverridingParentState = false;
	this->bChildrenNodesInitialized = false;
	this->bAssetClassComputed = false;
}

TSharedPtr<FAssetDumpTreeNode> FAssetDumpTreeNode::MakeChildNode()
{
	TSharedRef<FAssetDumpTreeNode> NewNode = MakeShareable(new FAssetDumpTreeNode());
	NewNode->ParentNode = SharedThis(this);
	NewNode->RootDirectory = RootDirectory;
	NewNode->bIsChecked = bIsChecked;
	// Don't inherit bIsLocked - only the parent folder should be locked, not children

	Children.Add(NewNode);
	return NewNode;
}

FString FAssetDumpTreeNode::ComputeAssetClass()
{
	if (!bIsLeafNode)
	{
		return TEXT("");
	}

	FString FileContentsString;
	if (!FFileHelper::LoadFileToString(FileContentsString, *DiskPackagePath))
	{
		UE_LOG(LogAssetGenerator, Error, TEXT("Failed to load asset dump json file %s"), *DiskPackagePath);
		return TEXT("Unknown");
	}

	// Try quick and dirty method that saves us time from parsing JSON fully
	const FString AssetClassPrefix = TEXT("\"AssetClass\": \"");
	const int32 AssetClassIndex = FileContentsString.Find(*AssetClassPrefix);
	if (AssetClassIndex != INDEX_NONE)
	{
		return CollectQuotedString(FileContentsString, AssetClassIndex + AssetClassPrefix.Len());
	}

	UE_LOG(LogAssetGenerator, Warning, TEXT("Quick AssetClass computation method failed for asset file %s"), *DiskPackagePath);

	// Fallback to heavy json file scanning
	TSharedPtr<FJsonObject> JsonObject;
	if (FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(FileContentsString), JsonObject))
	{
		return JsonObject->GetStringField(TEXT("AssetClass"));
	}

	UE_LOG(LogAssetGenerator, Error, TEXT("Failed to parse asset dump file %s as valid json"), *DiskPackagePath);
	return TEXT("Unknown");
}

void FAssetDumpTreeNode::SetupPackageNameFromDiskPath()
{
	// Remove extension from the file path (all asset dump files are json files)
	FString PackageNameNew = FPaths::ChangeExtension(DiskPackagePath, TEXT(""));

	// Make path relative to root directory (e.g D:\ProjectRoot\DumpRoot\Game\FactoryGame\Asset -> Game\FactoryGame\Asset)
	const FString RootDirectorySlash = RootDirectory / TEXT("");
	FPaths::MakePathRelativeTo(PackageNameNew, *RootDirectorySlash);

	// Normalize path separators to use / instead of backslashes (Game/FactoryGame/Asset)
	PackageNameNew.ReplaceInline(TEXT("\\"), TEXT("/"), ESearchCase::CaseSensitive);

	// Make sure package path starts with a forward slash (/Game/FactoryGame/Asset)
	PackageNameNew.InsertAt(0, TEXT('/'));

	// Make sure package name is sanitised before using it
	PackageNameNew = UPackageTools::SanitizePackageName(PackageNameNew);

	this->PackageName = PackageNameNew;
	this->NodeName = PackageNameNew.Len() > 1 ? FPackageName::GetShortName(PackageNameNew) : PackageNameNew;
}

FString FAssetDumpTreeNode::GetOrComputeAssetClass()
{
	if (!bAssetClassComputed)
	{
		this->AssetClass = ComputeAssetClass();
		this->bAssetClassComputed = true;
	}
	return AssetClass;
}

void FAssetDumpTreeNode::RegenerateChildren()
{
	if (bIsLeafNode)
	{
		return;
	}
	IPlatformFile &PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	TArray<FString> ChildDirectoryNames;
	TArray<FString> ChildFilenames;

	PlatformFile.IterateDirectory(*DiskPackagePath, [&](const TCHAR *FilenameOrDirectory, bool bIsDirectory)
								  {
		if (bIsDirectory) {
			ChildDirectoryNames.Add(FilenameOrDirectory);
		//TODO this should really use a better filtering mechanism than checking file extension, or maybe we could use some custom extension like .uassetdump
		} else if (FPaths::GetExtension(FilenameOrDirectory) == TEXT("json")) {
			ChildFilenames.Add(FilenameOrDirectory);
		}
		return true; });

	// Append child directory nodes first, even if they are empty
	for (const FString &ChildDirectoryName : ChildDirectoryNames)
	{
		const TSharedPtr<FAssetDumpTreeNode> ChildNode = MakeChildNode();
		ChildNode->bIsLeafNode = false;
		ChildNode->DiskPackagePath = ChildDirectoryName;
		ChildNode->SetupPackageNameFromDiskPath();
	}

	// Append filenames then, these represent individual packages
	for (const FString &AssetFilename : ChildFilenames)
	{
		const TSharedPtr<FAssetDumpTreeNode> ChildNode = MakeChildNode();
		ChildNode->bIsLeafNode = true;
		ChildNode->DiskPackagePath = AssetFilename;
		ChildNode->SetupPackageNameFromDiskPath();
	}
}

void FAssetDumpTreeNode::GetChildrenNodes(TArray<TSharedPtr<FAssetDumpTreeNode>> &OutChildrenNodes)
{
	if (!bChildrenNodesInitialized)
	{
		this->bChildrenNodesInitialized = true;
		RegenerateChildren();
	}
	OutChildrenNodes.Append(Children);
}

void FAssetDumpTreeNode::UpdateSelectedState(bool bIsCheckedNew, bool bIsSetByParent)
{
	this->bIsChecked = bIsCheckedNew;

	// We reset override state when selected state is updated by parent
	if (bIsSetByParent)
	{
		this->bIsOverridingParentState = false;
	}
	else
	{
		// Otherwise we reset it if it matches parent state or set it to true otherwise
		const TSharedPtr<FAssetDumpTreeNode> ParentNodePinned = ParentNode.Pin();
		const bool bIsParentChecked = ParentNodePinned.IsValid() ? ParentNodePinned->IsChecked() : false;
		// If updated state matches parent state, we should remove override
		if (bIsParentChecked == bIsCheckedNew)
		{
			this->bIsOverridingParentState = false;
		}
		else
		{
			// Otherwise our state differs from the parents one, so we are overriding it
			this->bIsOverridingParentState = true;
		}
	}

	// For directory nodes, ensure children are initialized before propagating state
	if (!bIsLeafNode && !bChildrenNodesInitialized)
	{
		this->bChildrenNodesInitialized = true;
		RegenerateChildren();
	}

	// Propagate state update to children widgets
	for (const TSharedPtr<FAssetDumpTreeNode> &ChildNode : Children)
	{
		ChildNode->UpdateSelectedState(bIsCheckedNew, true);
	}
}

void FAssetDumpTreeNode::PopulateGeneratedPackages(TArray<FName> &OutPackageNames, const TSet<FName> *WhitelistedAssetClasses)
{
	if (bIsLeafNode)
	{
		// If we represent leaf node, append our package name if we are checked
		if (bIsChecked)
		{
			if (WhitelistedAssetClasses == NULL || WhitelistedAssetClasses->Contains(*GetOrComputeAssetClass()))
			{
				OutPackageNames.Add(*PackageName);
			}
		}
	}
	else
	{
		// Otherwise we represent a directory node
		// If this directory is unchecked, only process if it has children that are explicitly checked
		if (!bIsChecked)
		{
			// Initialize children if needed to check for overrides
			if (!bChildrenNodesInitialized)
			{
				RegenerateChildren();
			}

			// Check if any children are checked (overriding this directory's unchecked state)
			bool bHasCheckedChildren = false;
			for (const TSharedPtr<FAssetDumpTreeNode> &ChildNode : Children)
			{
				if (ChildNode->bIsChecked)
				{
					bHasCheckedChildren = true;
					break;
				}
			}

			// If no children are checked, skip this entire directory
			if (!bHasCheckedChildren)
			{
				return;
			}
		}
		else
		{
			// Directory is checked, ensure children are initialized
			if (!bChildrenNodesInitialized)
			{
				RegenerateChildren();
			}
		}

		// Process all children - they will individually check if they should be included
		for (const TSharedPtr<FAssetDumpTreeNode> &ChildNode : Children)
		{
			ChildNode->PopulateGeneratedPackages(OutPackageNames, WhitelistedAssetClasses);
		}
	}
}

TSharedPtr<FAssetDumpTreeNode> FAssetDumpTreeNode::CreateRootTreeNode(const FString &DumpDirectory)
{
	const TSharedPtr<FAssetDumpTreeNode> RootNode = MakeShareable(new FAssetDumpTreeNode);

	RootNode->bIsLeafNode = false;
	RootNode->RootDirectory = DumpDirectory;
	RootNode->DiskPackagePath = RootNode->RootDirectory;
	RootNode->SetupPackageNameFromDiskPath();
	RootNode->UpdateSelectedState(true, false);

	return RootNode;
}

void SAssetDumpViewWidget::Construct(const FArguments &InArgs)
{
	ChildSlot[SNew(SVerticalBox) + SVerticalBox::Slot().AutoHeight().Padding(5.0f)[SNew(SHorizontalBox) + SHorizontalBox::Slot().AutoWidth()[SNew(SButton).Text(LOCTEXT("AssetGenerator_ToggleAll", "Toggle All")).OnClicked(this, &SAssetDumpViewWidget::OnToggleAllClicked)] + SHorizontalBox::Slot().AutoWidth().Padding(5.0f, 0.0f)[SNew(SButton).Text(LOCTEXT("AssetGenerator_ToggleSelected", "Toggle Selected (Space)")).OnClicked(this, &SAssetDumpViewWidget::OnToggleSelectedClicked)]] + SVerticalBox::Slot().FillHeight(1.0f)[SAssignNew(TreeView, STreeView<TSharedPtr<FAssetDumpTreeNode>>).HeaderRow(SNew(SHeaderRow) + SHeaderRow::Column(TEXT("ShouldGenerate")).DefaultLabel(LOCTEXT("AssetGenerator_ColumnShouldGenerate", "Generate")).FixedWidth(70).HAlignCell(HAlign_Center).HAlignHeader(HAlign_Center) + SHeaderRow::Column(TEXT("Path")).DefaultLabel(LOCTEXT("AssetGenerator_ColumnPath", "Asset Path")) + SHeaderRow::Column(TEXT("AssetClass")).DefaultLabel(LOCTEXT("AssetGenerator_ColumnAssetClass", "Asset Class")).FixedWidth(100).HAlignCell(HAlign_Left).HAlignHeader(HAlign_Center)).SelectionMode(ESelectionMode::Multi).OnGenerateRow_Raw(this, &SAssetDumpViewWidget::OnCreateRow).OnGetChildren_Raw(this, &SAssetDumpViewWidget::GetNodeChildren).TreeItemsSource(&this->RootAssetPaths)]];
}

void SAssetDumpViewWidget::SetAssetDumpRootDirectory(const FString &RootDirectory)
{
	this->RootNode = FAssetDumpTreeNode::CreateRootTreeNode(RootDirectory);
	this->RootAssetPaths.Empty();
	this->RootNode->GetChildrenNodes(RootAssetPaths);
	this->TreeView->RequestTreeRefresh();
}

void SAssetDumpViewWidget::PopulateSelectedPackages(TArray<FName> &OutPackageNames, const TSet<FName> *WhitelistedAssetClasses) const
{
	this->RootNode->PopulateGeneratedPackages(OutPackageNames, WhitelistedAssetClasses);
}

TSharedRef<ITableRow> SAssetDumpViewWidget::OnCreateRow(const TSharedPtr<FAssetDumpTreeNode> TreeNode,
														const TSharedRef<STableViewBase> &Owner) const
{
	return SNew(SAssetDumpTreeNodeRow, Owner, TreeNode);
}

void SAssetDumpViewWidget::GetNodeChildren(const TSharedPtr<FAssetDumpTreeNode> TreeNode, TArray<TSharedPtr<FAssetDumpTreeNode>> &OutChildren) const
{
	TreeNode->GetChildrenNodes(OutChildren);
}

void SAssetDumpTreeNodeRow::Construct(const FArguments &InArgs, const TSharedRef<STableViewBase> &OwnerTable, const TSharedPtr<FAssetDumpTreeNode> &TreeNodeArg)
{
	this->TreeNode = TreeNodeArg;
	FSuperRowType::Construct(FTableRowArgs(), OwnerTable);
}

TSharedRef<SWidget> SAssetDumpTreeNodeRow::GenerateWidgetForColumn(const FName &InColumnName)
{
	if (InColumnName == TEXT("Path"))
	{
		return SNew(SHorizontalBox) + SHorizontalBox::Slot().AutoWidth()[SNew(SExpanderArrow, SharedThis(this))] + SHorizontalBox::Slot().FillWidth(1.0f)[SNew(STextBlock).Text(FText::FromString(TreeNode->NodeName))];
	}
	if (InColumnName == TEXT("AssetClass"))
	{
		if (TreeNode->bIsLeafNode)
		{
			return SNew(STextBlock)
				.Text(FText::FromString(TreeNode->GetOrComputeAssetClass()));
		}
		return SNullWidget::NullWidget;
	}

	// Capture TreeNode by value to keep it alive even if the row is destroyed
	TSharedPtr<FAssetDumpTreeNode> TreeNodePtr = TreeNode;

	return SNew(SCheckBox)
		.IsChecked_Lambda([TreeNodePtr]()
						  { return TreeNodePtr.IsValid() && TreeNodePtr->IsChecked() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked; })
		.OnCheckStateChanged_Lambda([TreeNodePtr](ECheckBoxState NewState)
									{
            if (TreeNodePtr.IsValid())
            {
                const bool bIsChecked = NewState == ECheckBoxState::Checked;
                TreeNodePtr->UpdateSelectedState(bIsChecked, false);
            } });
}

FReply SAssetDumpViewWidget::OnToggleAllClicked()
{
	if (!RootNode.IsValid() || !TreeView.IsValid())
	{
		return FReply::Handled();
	}

	// Determine new state based on first child's state (toggle children, not root)
	TArray<TSharedPtr<FAssetDumpTreeNode>> Children;
	RootNode->GetChildrenNodes(Children);

	if (Children.Num() > 0)
	{
		const bool bNewState = !Children[0]->IsChecked();

		// Recursively update all descendants
		TFunction<void(const TSharedPtr<FAssetDumpTreeNode> &, bool)> UpdateNodeAndDescendants;
		UpdateNodeAndDescendants = [&UpdateNodeAndDescendants](const TSharedPtr<FAssetDumpTreeNode> &Node, bool bState)
		{
			if (!Node.IsValid())
			{
				return;
			}

			Node->UpdateSelectedState(bState, false);

			// Get children and recursively update them
			TArray<TSharedPtr<FAssetDumpTreeNode>> NodeChildren;
			Node->GetChildrenNodes(NodeChildren);
			for (const TSharedPtr<FAssetDumpTreeNode> &Child : NodeChildren)
			{
				if (Child.IsValid())
				{
					UpdateNodeAndDescendants(Child, bState);
				}
			}
		};

		// Update only the children, not the root node itself
		for (const TSharedPtr<FAssetDumpTreeNode> &ChildNode : Children)
		{
			if (ChildNode.IsValid())
			{
				UpdateNodeAndDescendants(ChildNode, bNewState);
			}
		}
	}

	// Refresh the tree view to update all checkboxes
	if (TreeView.IsValid())
	{
		TreeView->RequestTreeRefresh();
	}

	return FReply::Handled();
}

FReply SAssetDumpViewWidget::OnToggleSelectedClicked()
{
	if (!TreeView.IsValid())
	{
		return FReply::Handled();
	}

	TArray<TSharedPtr<FAssetDumpTreeNode>> SelectedItems = TreeView->GetSelectedItems();
	for (const TSharedPtr<FAssetDumpTreeNode> &Node : SelectedItems)
	{
		if (Node.IsValid())
		{
			Node->UpdateSelectedState(!Node->IsChecked(), false);
		}
	}

	if (TreeView.IsValid())
	{
		TreeView->RequestTreeRefresh();
	}

	return FReply::Handled();
}

FReply SAssetDumpViewWidget::OnKeyDown(const FGeometry &MyGeometry, const FKeyEvent &InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::SpaceBar && TreeView.IsValid())
	{
		return OnToggleSelectedClicked();
	}

	// Let parent handle the key event
	return SCompoundWidget::OnKeyDown(MyGeometry, InKeyEvent);
}

#undef LOCTEXT_NAMESPACE