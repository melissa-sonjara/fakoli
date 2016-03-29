<?php
Fakoli::using("html_editor");

$snippet_id = checkNumeric($_GET["snippet_id"]);
if (!$snippet_id) Fakoli::end("");

$snippet = new Snippet($snippet_id);
echo $snippet->snippet;