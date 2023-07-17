<?php
Fakoli::using("scroll");

if (!checkRole("admin"))
{
	Fakoli::end("Access Denied");
}

$scroll_id = checkNumeric($_GET["scroll_id"]);

if (!$scroll_id)
{
	Fakoli::end("Missing or invalid parameter");
}

$from = new Scroll($scroll_id);

$scroll = new Scroll();
$scroll->filter = new InclusionFilter("identifier");

$form = new AutoForm($scroll, "POST", "/action/scroll/copy_scroll_dialog?scroll_id=$scroll_id");
$form->unique("identifier");
$form->submitLabel = "Copy Scroll";

$form->ajaxSubmit("function(result) {ScrollPartManager.onCopyScroll(result);}", "function() {document.id('{$form->id}_error').set('text','Failed to communicate with server'); }");
$form->button("Cancel", "ModalDialog.closeAllDialogs()", null, true);

if ($method == "POST")
{
	if ($form->save())
	{
		Fakoli::end("OK");
	}
	else
	{
		Fakoli::end($form->msg);
	}
}

echo $form->writeScript();
$form->drawForm();