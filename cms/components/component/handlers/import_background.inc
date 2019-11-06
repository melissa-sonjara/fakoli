<?php
Fakoli::using("process");

if (!$process)
{
	// We are in a user request - spawn the background process and return the ID
	
	$process = new ProcessManager();
	$process->put("xml_import_file_path", $_SESSION["xml_import_file_path"]);
	$process->put("xml_import_components", $_SESSION["xml_import_components"]);
	echo $process->run("/action/component/import_background");
}
else
{
	// We are in the spawned background process - begin indexing the library
	
	$mgr = new SerializationManager();
	$mgr->importBackground();
}
?>