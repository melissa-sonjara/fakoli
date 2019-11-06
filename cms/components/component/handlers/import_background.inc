<?php
Fakoli::using("process");

if (!$process)
{
	// We are in a user request - spawn the background process and return the ID
	
	$process = new ProcessManager();
	$params = [ "xml_import_file_path" =>$_SESSION["xml_import_file_path"],
				"xml_import_components", $_SESSION["xml_import_components"]
	];
	echo $process->run("/action/component/import_background", $params);
}
else
{
	// We are in the spawned background process - begin indexing the library
	
	$mgr = new SerializationManager();
	$mgr->importBackground();
}
?>