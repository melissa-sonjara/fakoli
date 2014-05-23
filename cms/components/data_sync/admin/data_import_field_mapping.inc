<?php
Fakoli::using("data_sync");
Fakoli::usingFeature("spreadsheet_form");

$class = $_SESSION["data_import_class"];

$menu_item = "Data Import";
$page_title = "Choose Fields to Import";

$profile_id = $_GET["import_profile_id"];

$multiProfile = Settings::getValue("data_sync", "multiple_import_profiles");

if ($multiProfile)
{

	if (!$profile_id)
	{
		$profile_id = $_SESSION["import_profile_id"];
		if ($profile_id) redirect("data_import_field_mapping?import_profile_id=$profile_id");
	}

	if ($profile_id)
	{
		$profile = new DataImportProfile($profile_id);
		if (!$profile->exists())
		{
			unset($_SESSION["import_profile_id"]);
			redirect("data_import_field_mapping");
		}
	}
		
	$profiles = Query::create(DataImportProfile, "WHERE class=:c ORDER BY profile")
					 ->bind(":c", $class)
					 ->execute();

	if (count($profiles) && !$profile_id)
	{
		$profile_id = $profiles[0]->import_profile_id;
	}

	$_SESSION["import_profile_id"] = $profile_id;
}
else
{
	$profile_id = 0;
}


$columns = DataSyncManager::getImportColumns($class);
$fieldMappings = DataSyncManager::getFieldMappings($class, $profile_id);

$options = array_combine($columns, $columns);

$mapping = new DataImportFieldMapping();
$form = new AutoForm($mapping);
$form->hide("class", "import_profile_id");
$columnSelect = new ChecklistFieldRenderer($form, "import_column", "Import Column", $options);
$columnSelect->setSize(250, 200);
$columnSelect->dropdown = true;
$columnSelect->dropdownMessage = "Click to Select Columns";

$form->readonly("client_field");
$form->getRenderer("client_field")->template = "prettify";

$spreadsheet = new SpreadsheetForm($form, $fieldMappings, 0);

$spreadsheet->submitLabel = "Save Field Mapping";
$spreadsheet->button("Clear Field Mapping", "/action/data_sync/clear_mapping?import_profile_id={$profile_id}");

$tabs = DataSyncManager::getAdminImportTabs();

if ($method == "POST")
{
	if ($spreadsheet->save())
	{
		$_SESSION["import_profile_id"] = $profile_id;
		$tabs->next();
	}
}

$script .= $spreadsheet->writeScript();

if ($multiProfile)
{
	if (!$profile_id) $profile_id = 0;
	
	$script .= <<<ENDSCRIPT
	<script type="text/javascript" src="/components/data_sync/js/data_sync_profile_manager.js"></script>
	<script type="text/javascript">
			
	var importProfileManager;

	
	window.addEvent('load', function()
	{
		importProfileManager = new DataImportProfileManager("{$class}", {$profile_id});
	});
	</script>
ENDSCRIPT;
}

$tabs->writeHTML();
?>
<div class="tab_border">
<?

if ($multiProfile)
{
?>
<label for="import_profile_id">Select Import Profile: </label><select name="import_profile_id" onchange='go("?import_profile_id=" + this.value);'>
<?
foreach($profiles as $profile)
{
	option($profile->import_profile_id, $profile->profile, $profile_id);
}
?>
</select>&nbsp;<a href='#' class='button' onclick='importProfileManager.createProfileDialog(); return false;'>Create New Profile</a>
<br/>	
<br/>
<?
}
$spreadsheet->drawForm();
?>
</div>