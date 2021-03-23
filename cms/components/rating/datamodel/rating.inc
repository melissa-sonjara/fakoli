<?php
/**\addtogroup rating
 * @{
 */



class PageRating extends DataItem
{
	var $table = "rating";
	var $primary_key = "rating_id";
	
	var $fields = array("rating_id"		=>	Number,
						"url"			=>	String,
						"user_id"		=>	Number,
						"rating"		=>	Number,
						"rating_date"	=>	Timestamp);
	
	static function fromURL($url, $user_id = 0)
	{
		global $user;
		
		if (!$user_id) $user_id = $user->get($user->getPrimaryKey());
		
		try 
		{
			$rating = Query::create(PageRating, "WHERE url=:u AND user_id=:id")
						   ->bind(":u", $url, ":id", $user_id)
						   ->executeSingle();		
		} 
		catch (DataNotFoundException $e) 
		{
			$rating = new PageRating();
			$rating->url = $url;
			$rating->user_id = $user_id;
		}
		
		return $rating;
	}
}


class PageRatingStatistics extends DataItem
{
	var $table = "rating_statistics";
	
	var $fields = array("url"				=>	String,
						"average_rating"	=>	Number,
						"number_of_ratings"	=>	Number);
	
	static function fromURL($url)
	{
		try
		{
			$stats = Query::create(PageRatingStatistics, "WHERE url=:u")
						  ->bind(":u", $url)
						  ->executeSingle();
		}
		catch(DataNotFoundException $e)
		{
			$stats = new PageRatingStatistics();
			$stats->url = $url;
			$stats->average_rating = 0.0;
			$stats->number_of_ratings = 0;
		}

		return $stats;
	}
}
/**@}*/?>