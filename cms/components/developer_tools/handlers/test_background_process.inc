<?php
Fakoli::using("process", "developer_tools");

if (!checkRole("admin"))
{
	throw new FakoliException("Permission Denied");
}

if (!$process)
{
	// We are in a user request - spawn the background process and return the ID

	$process = new ProcessManager();

	echo $process->run("/action/developer_tools/test_background_process");
}
else
{
	for($i = 0; $i < 100; $i += 10)
	{
		$process->setProgress("Running", "Testing", $i);
		sleep(3);
	}
	
	$process->setProgress("Completed", "Test Completed", 100);
}
?>