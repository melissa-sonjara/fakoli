<?php
Fakoli::using("html_editor");

$snippet_id = checkNumeric($_GET["snippet_id"]);

if ($snippet_id)
{
	$snippet = new Snippet($snippet_id);
?>
	<p><?echo $snippet->description?></p>
	<div class='snippet_preview'>
	<?echo $snippet->snippet?>
	</div>
	<a class='button' href='#' onclick='new SnippetManager().insertSnippet(<?echo $snippet_id?>); return false;'>Insert Snippet</a>&nbsp;&nbsp;
	<a class='button' href='#' onclick='new SnippetManager().showSnippetDialog(<?echo $snippet_id?>); return false;'>Edit Snippet</a>&nbsp;&nbsp;
	<a class='button' href='#' onclick='new SnippetManager().showSnippetDialog(); return false;'>Create a New Snippet</a>&nbsp;&nbsp;
	<a class='button' href='#' onclick='new SnippetManager().hide(); return false;'>Cancel</a>
<?
}
else
{
?>
<p>Create or select a snippet to continue</p>
<div class='snippet_preview'></div>
<a class='button' href='#' onclick='new SnippetManager().showSnippetDialog(); return false;'>Create a New Snippet</a>&nbsp;&nbsp;
<a class='button' href='#' onclick='new SnippetManager().hide(); return false;'>Cancel</a>
<?
}