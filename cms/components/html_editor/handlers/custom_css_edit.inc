<?php
Fakoli::using("html_editor", "settings");

$editor = checkIdentifier($_GET["editor"]);

if (!checkRole("admin"))
{
	Fakoli::end("Permission Denied");
}

$c = Component::getComponent("html_editor");

$form = new SettingAutoForm($c, new InclusionFilter("enable_custom_CSS", "custom_CSS"));
$form->setLayout(new UngroupedFormLayout($form));

$form->action = "/action/html_editor/custom_css_edit?editor=" . $editor;

$form->ajaxSubmit("function (response){new CustomCSSManager().cssEditorResponse(response); }");

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

$script .= $form->writeScript();
$form->drawForm();