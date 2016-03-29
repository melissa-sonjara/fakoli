<?php
Fakoli::using("html_editor");

$snippet_id = checkNumeric($_GET["snippet_id"]);

$snippets = Query::create(Snippet, "WHERE user_id=:u OR shared=1 ORDER BY snippet_name")
				 ->bind(":u", $user->getPrimaryKeyValue())
				 ->execute();

$panel = new Panel("snippet_panel", "/action/html_editor/snippet_panel?snippet_id=$snippet_id");

?>
<label for='snippet_id'>Select Snippet </label>
<select id='snippet_id' onchange="new SnippetManager().updateSnippetPanel(this.value); return false;">
 <option value='0'>Select a Snippet...</option>
<?
foreach($snippets as $snippet)
{
	option($snippet->snippet_id, $snippet->snippet_name, $snippet_id);
}
?>
</select>
<br/>
<?
$panel->draw();
?>
