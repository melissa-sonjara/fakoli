<?php
/**\addtogroup social_media
 * @{
 */


Fakoli::using("settings");

class SocialMediaManager
{
	static function setDefaults()
	{
		Settings::setDefaultValue("social_media", "Facebook", true, 'Boolean', "", "Chicklets");
		Settings::setDefaultValue("social_media", "Twitter", true, 'Boolean', "", "Chicklets");
		Settings::setDefaultValue("social_media", "Digg", true, 'Boolean', "", "Chicklets");
		Settings::setDefaultValue("social_media", "StumbleUpon", true, 'Boolean', "", "Chicklets");
		Settings::setDefaultValue("social_media", "Del.icio.us", true, 'Boolean', "", "Chicklets");
		Settings::setDefaultValue("social_media", "Reddit", true, 'Boolean', "", "Chicklets");
		Settings::setDefaultValue("social_media", "Yahoo_MyWeb", false, 'Boolean', "", "Chicklets");
		Settings::setDefaultValue("social_media", "Newsvine", false, 'Boolean', "", "Chicklets");
		Settings::setDefaultValue("social_media", "Google_Bookmarks", false, 'Boolean', "", "Chicklets");
		Settings::setDefaultValue("social_media", "Mixx", false, 'Boolean', "", "Chicklets");
		Settings::setDefaultValue("social_media", "Technorati", false, 'Boolean', "", "Chicklets");
	}
}

/**@}*/?>
