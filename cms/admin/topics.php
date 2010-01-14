<?php

require_once "../../include/config.inc";

require_once "../datamodel/topic.inc";
require_once "../../include/permissions.inc";

$menu_item = "Topics";
$title = "Manage Topics";

$topics = query(Topic, "ORDER BY topic_id");

require_once admin_begin_page;
?>
<table class="list" width="100%">
 <tr>
  <th>Topic</th>
 </tr>
<?
foreach($topics as $topic)
{
?>
 <tr>
  <td><b><a href="topic_form.php?topic_id=<?echo $topic->topic_id ?>"><?echo $topic->topic ?></a></b></td>

 </tr>
<?
}
?>
</table>
<br/>
<br/>
<a href="topic_form.php" class="button"> Add a New Topic </a>
<br/>
<br/>
<?
require_once admin_end_page;
?>