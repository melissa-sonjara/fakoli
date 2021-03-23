<?php
/**\addtogroup questionnaire
 * @{
 */


/**
 * Provides methods for managing the questionniare
 * component.
 * 
 * Methods for managing the Questionnaire feature interface
 * are in QuestionnaireManager classes.
 * 
 * @author janice
 *
 */
class QuestionnaireComponentManager
{	
	static function upgradeComponent($version)
	{
		$mgr = new QuestionnaireUpgradeManager();
		$mgr->upgrade($version);
	}	
}

/**@}*/?>