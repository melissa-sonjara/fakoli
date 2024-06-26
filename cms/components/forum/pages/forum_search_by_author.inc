<?php
Fakoli::using("forum", "user");
Fakoli::usingFeature("join");

$text = $_GET["author"];
$forum_id = checkNumeric($_GET["forum_id"]);

$mgr = new UserManager();
$cl = $mgr->getUserClass();
$u = new $cl;
if (!($u->hasField("first_name") && $u->hasField("last_name")))
{
	throw new FakoliException("User object model does not support search by name");
}

$pk = $u->getPrimaryKey();
$table = $u->table;
$text = "%{$text}%";

$messages = Query::create(ForumMessage, "WHERE deleted=0 AND forum_id=:f AND author_id IN (SELECT $pk FROM $table WHERE concat(first_name, ' ', last_name) LIKE :t) ORDER BY date_posted DESC")
				 ->bind(":f", $forum_id, ":t", $text)
    			 ->execute();
		
$view = new ForumSearchResultsView($messages, $_GET["author"]);

$script .= $view->writeScript();
$view->drawView();