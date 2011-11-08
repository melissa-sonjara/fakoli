<?php
Fakoli::using("activity_tracker", "case", "swf");
Fakoli::usingFeature("data_view");

$session_id = $_GET["session_id"];
$showImages = checkNumeric($_GET["showImages"]);

$menu_item = "Activity Report";

class SessionReportHelper
{
	var $feedbackMsgs;
	var $feebackActions;
	
	function SessionReportHelper($session_id)
	{
		$this->feedbackMsgs = Query::create(Feedback, "WHERE session=:session ORDER BY created_date")
									->bind(":session", $session_id)
									->execute();
									
		$this->feedbackActions = Query::create(UserActivity, "WHERE session_id=:session AND uri='/action/feedback/save' ORDER BY activity_time")
									->bind(":session", $session_id)
									->execute();
	}
	
	function onStartRow($activity)
	{
		$uri = $activity->uri;
		
		if (startsWith($uri, "/action/feedback/save")) return "feedback";
		if (startsWith($uri, "/case_view") || startsWith($uri, "/task_list")) return "prototype_page";
		if (startsWith($uri, "/action/image")) return "image_action";
		if (startsWith($uri, "/action")) return "action";
		
		return "";
	}
	
	function formatURI($activity)
	{
		$uri = $activity->uri;
		
		$out = $uri;
		
		$matches = array();
		
		if (preg_match("|/case_view\\?patent_case_id=(\\d+)|", $uri, $matches))
		{
			return "Open Case View for Patent Case {$matches[1]}";
		}
		
		if (startsWith($uri, "/task_list")) return "Open Examiner Dashboard";
		if (startsWith($uri, "/action/feedback/save")) return "User Feedback";
		if (startsWith($uri, "/action/case_view/contents")) return "Case Folder: Contents Tab";
		if (startsWith($uri, "/action/case_view/bib_data")) return "Case Folder: Case Data Tab";
		if (startsWith($uri, "/action/case_view/case_history")) return "Case Folder: Case History Tab";
		if (startsWith($uri, "/action/case_view/searches")) return "Case Folder: Searches Tab";
		if (startsWith($uri, "/action/case_view/related_cases")) return "Case Folder: Related Cases Tab";
		if (startsWith($uri, "/action/case_view/references")) return "Case Folder: References Tab";
		if (startsWith($uri, "/action/case_view/docs")) return "Case Folder: Contents Tab Documents";
		
		if (startsWith($uri, "/action/case_view/empty_item")) return "Open Empty Panel in Slot ".($this->getSlot($uri));
		if (startsWith($uri, "/action/case_view/image_item")) return "Open Image Document '".($this->getDocTitle($uri))."' in Slot ".($this->getSlot($uri));
		if (startsWith($uri, "/action/case_view/claim_item")) return "Open Claim Tree in Slot ".($this->getSlot($uri));
		if (startsWith($uri, "/action/case_view/text_item")) return "Open Text Document '".($this->getDocTitle($uri))."' in Slot ".($this->getSlot($uri));
		if (startsWith($uri, "/action/swf/video?")) return "Watch Video '".($this->getVideoTitle($uri))."'";
		return $out;
	}
	
	function formatMessage($activity)
	{
		$uri = $activity->uri;
		$out = "";
		if (startsWith($uri, "/action/feedback/save"))
		{
			$idx = 0;
			foreach($this->feedbackActions as $action)
			{
				if ($action->activity_id == $activity->activity_id)
				{
					$out = "<span style='font-weight: normal'>".$this->feedbackMsgs[$idx]->feedback."</span>";
					break;
				}
				
				++$idx;
			}

		}
		return $out;
		
	}
	
	function getSlot($uri)
	{
		$matches = array();
		
		preg_match("/slot=slot(\\d)/", $uri, $matches);
		
		return $matches[1];
	}
	
	function getDocTitle($uri)
	{
		$matches = array();
		preg_match("/patent_case_document_id=(\\d+)/", $uri, $matches);
		
		$doc = Query::create(PatentCaseDocument, "WHERE patent_case_document_id=:id")
						->bind(":id", $matches[1])
						->execute();
						
		return $doc[0]->title;
	}
	
	function getVideoTitle($uri)
	{
		$matches = array();
		preg_match("/video_id=(\\d+)/", $uri, $matches);
		
		$doc = Query::create(Video, "WHERE video_id=:id")
						->bind(":id", $matches[1])
						->execute();
						
		return $doc[0]->title;
	}
						
}

$title = "Session Details for $session_id";

$helper = new SessionReportHelper($session_id);

if (!$showImages)
{
	$constraints .= "AND uri NOT LIKE '/action/image/%'";
}


$activity = Query::create(UserActivity, "WHERE session_id=:id $constraints ORDER BY activity_time ASC")
				 ->bind(":id", $session_id)
				 ->execute();

$table = new DataListView($activity, "activity_table");
$table->column("Date", "{activity_time}", true, "width: 15%")
	  ->column("URI", array($helper, formatURI), true)
	  ->column("Message", array($helper, formatMessage), true);
	  
$table->filter = true;
$table->pageSize = 0;
$table->paginate = false;
$table->sortable = true;
$table->zebra = false;
$table->onStartRow = array($helper, onStartRow);

$table->excelFile = "session_{$session_id}.xls";

$script .= $table->writeScript();
?>
<form method="GET" action="">
<input type='hidden' name='session_id' value="<?echo $session_id?>"/>
<input type="checkbox" class="checkbox" name="showImages" id="showImages" value="1"<?echo $showImages ? " checked='checked'" : ""?>/> Show Image Records&nbsp;&nbsp;
<input type="submit" class="button" value=" GO "/>
</form>
<?
$table->drawView();
?>