<?php
Fakoli::using("html_editor");

if (Settings::getValue("html_editor", "enable_custom_CSS"))
{
	header("Content-type: text/css");
	Fakoli::end(Settings::getValue("html_editor", "custom_CSS"));
}