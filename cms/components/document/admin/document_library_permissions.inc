<?

Fakoli::using("document", "role");
Fakoli::usingFile("framework/auto_form.inc");

require_once "include/document_library_tabs.inc";

$document_library_id = checkNumeric($_GET["document_library_id"]);

if (!$document_library_id)
{
	redirect("/admin/document_libraries");
}

$library = new DocumentLibrary($document_library_id);

$library->filter = new InclusionFilter("document_library_id", "manage_folders", "upload_files", "delete_files", "last_modified");

$tabs = documentLibraryTabs($document_library_id);

$form = new AutoForm($library);

$manageFoldersList = new CheckListFieldRenderer($form, "manage_folders", "Create & Delete Folders", SiteRole::getRolesArray());
$uploadFilesList   = new CheckListFieldRenderer($form, "upload_files", "Upload Files", SiteRole::getRolesArray());
$deleteFilesList   = new CheckListFieldRenderer($form, "delete_files", "Delete Files", SiteRole::getRolesArray());

if ($method == "POST")
{
	if ($form->save())
	{
		$tabs->next();
	}
}

$script .= $form->writeScript();

$tabs->writeHTML();
?>
<div style="clear:left; border: solid 1px #000; width: 100%; margin: 0; padding: 4px;">
<h3>Permissions for <?echo $library->name?></h3>
<?
$form->drawForm();
?>
</div>