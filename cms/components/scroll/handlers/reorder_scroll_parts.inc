<?php
Fakoli::using("scroll");

Fakoli::assertRole("admin,editor");

try
{
	updateSortOrder('ScrollPart');
	Fakoli::end("OK");
}
catch(Exception $e)
{
	Fakoli::end($e->getMessage());
}

