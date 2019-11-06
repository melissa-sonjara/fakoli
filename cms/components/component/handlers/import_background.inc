<?php
Fakoli::using("process");

if (!$process)
{
	// We are in a user request - spawn the background process and return the ID
	
	$process = new ProcessManager();
	echo $process->run("/action/component/import_background");
}
else
{
	// We are in the spawned background process - begin indexing the library
	
	$mgr = new SerializationManager();
	$mgr->importBackground();
}
?>