<?php
Fakoli::using("data_sync", "process");

checkRole("admin");

if (!$process)
{
	// We are in a user request - spawn the background process and return the ID

	$process = new ProcessManager();
	$file =  sanitizePath($_SESSION["data_import_file"]);
	chmod($file, 0777);
	
	$data_file_format = $_SESSION["data_file_format"];
	
	$class = $_SESSION["data_import_class"];
	$import_profile_id = $_SESSION["import_profile_id"];
	
	$new = checkNumeric($_GET["new"]);
	$matching = checkNumeric($_GET["matching"]);
	
	
	echo $process->run("/action/data_sync/batch_import?data_import_file={$file}&new={$new}&matching={$matching}&data_import_class={$class}&import_profile_id={$import_profile_id}&data_file_format={$data_file_format}");
}
else
{
	$file =  sanitizePath($_GET["data_import_file"]);
	$class = checkIdentifier($_GET["data_import_class"]);
	$import_profile_id = checkNumeric($_GET["import_profile_id"]);
	$format = checkIdentifier($_GET["data_file_format"]);
	
	$new = checkNumeric($_GET["new"]);
	$matching = checkNumeric($_GET["matching"]);

	DataSyncManager::batchImportItems($class, $import_profile_id, $file, $matching, $new, $process, $format);
}
?>