<?php
Fakoli::using("activity_tracker", "process");
Fakoli::assertRole("admin");

if (!$process)
{
	// We are in a user request - spawn the background process and return the ID
	
	$process = new ProcessManager();
	echo $process->run("/action/activity_tracker/migrate_data");
}
else
{
	echo "Executing migration script";
	
	try
	{
		$process->setProgress("Running", "Materializing Session Records", 5);
		
		$db = ConnectionManager::getConnection();
		$db->exec("INSERT INTO user_activity_session (session_identifier, user_id, session_start, session_end, request_count) select session_id, user_id, session_start, session_end, page_views from activity_sessions");
		
		$process->setProgress("Running", "Retrieving Session summaries", 10);
		
		$sessions = Query::create(ActivitySession, "")->execute();
		
		$max = count($sessions);
		$count = 1;
		
		foreach($sessions as $session)
		{
			$process->setProgress("Running", "Migrating $count of $max", number_format(10 + ($count * 90 / $max), 1));
			ActivityTracker::migrateData($session);
			++$count;
		}

		$process->setProgress("Completed", "All done!", 100);
	}
	catch(Exception $e)
	{
		$process->setProgress("Error", $e->getMessage(), 100);
	}
}

