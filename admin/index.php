<?php

require_once "../include/config.inc";

require_once "../include/permissions.inc";

if (!checkRole("admin"))
{
	redirect("/admin/login");
}
else
{
	redirect("/admin/index");
}
