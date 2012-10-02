<?php

class SurveyManagerManager
{
	function SurveyManagerManager()
	{
		
	}
	
	static function upgradeComponent($version)
	{
		$mgr = new SurveyManagerUpgradeManager();
		$mgr->upgrade($version);
	}
}