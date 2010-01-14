<?php

function drawTabs()
{
	$tabs = array(
				"My Tasks"	=> array("role" => "admin,employee", "page" => "index.php"),
				"Projects"	=> array("role" => "admin,employee,client", "page" =>	"projects.php"), 
				"Clients"	=> array("role" => "admin,employee", "page" => "clients.php"),
				"Personnel"	=> array("role" => "admin", "page" => "personnel.php"),
				"Timecard"	=> array("role" => "admin,employee", "page" => "timecard.php"),
				"Reports"	=> array("role" => "admin", "page" => "work_progress_report.php")
	);
	
	echo "<div style='display:inline; width: 100px'>&nbsp;</div>";
	foreach(array_keys($tabs) as $tab)
	{
		if ($tab == $section)
		{
			$tab_class = "current_tab";
		}
		else
		{
			$tab_class = "tab";
		}

		if (checkRole($tabs[$tab]["role"]))
		{
			echo "<span class='$tab_class'><a href='{$tabs[$tab]["page"]}'>$tab</a></span>\n";
		}
	}
}

?>