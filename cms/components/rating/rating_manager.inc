<?php
/**\addtogroup rating
 * @{
 */


/**
 *
 * Provides a central management class for event handlers and 
 * common functionality for the rating component.
 */

class RatingManager
{
	function __construct()
	{

	}

	static function setDefaults()
	{
		//TODO: Set default configuration parameters here
	}

	static function getCurrentUserPageRating()
	{
		global $user;
		$url = Fakoli::$requestURI;

		if (!$user) return null;
		
		try 
		{
			$rating = Query::create('PageRating', "WHERE url=:u and user_id=:id")
						   ->bind(":u", $url, ":id", $user->get($user->getPrimaryKey()))
						   ->executeSingle();
		} 
		catch (DataNotFoundException $e) 
		{
			return null;
		}
		
		return $rating;
	}
	
	static function getCurrentPageRatingStatistics()
	{
		$url = Fakoli::$requestURI;
		
		try
		{
			$rating = PageRatingStatistics::fromURL($url);
		}
		catch(DataNotFoundException $e)
		{
			$rating = new PageRatingStatistics();
			$rating->url = $url;
		}
		
		return $rating;
	}
	
	static function drawRatingWidget($obj = null)
	{
		global $user;
		global $script;
		
		$ratingStats = RatingManager::getCurrentPageRatingStatistics();
		$userRating = RatingManager::getCurrentUserPageRating();
		
		$options = "readOnly: ". (($userRating || !$user) ? "true" : "false");
	
		if ($obj)
		{
			$name = $obj->prettifyClassName();
			$options .= ", title: 'Rate this $name', readOnlyTitle: '$name Rating', loginMessage: 'Log in to rate this $name'";
		}
			
		$url = Fakoli::$requestURI;
		
		$script .= <<<ENDSCRIPT
<script type='text/javascript'>
window.addEvent('load', function()
{
	new PageRating("page_rating_widget", '{$url}', {$ratingStats->average_rating}, {$ratingStats->number_of_ratings}, {{$options}});
});
</script>
ENDSCRIPT;
?>
<div id="page_rating_widget"></div>
<?
	}
	
	static function upgradeComponent($version)
	{
		$mgr = new RatingUpgradeManager();
		$mgr->upgrade($version);
	}
}

/**@}*/?>
