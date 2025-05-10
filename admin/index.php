<?php

require_once "cms/constants.inc";
require_once "../include/config.inc";
require_once "cms/core.inc";

if (!checkRole("admin"))
{
	redirect("/admin/login");
}
else
{
	redirect("/admin/index");
}

?>