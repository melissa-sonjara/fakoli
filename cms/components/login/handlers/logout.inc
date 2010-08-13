<?php
session_destroy();

if ($_SERVER['HTTP_REFERER']) 
{
	redirect($_SERVER['HTTP_REFERER']);
}
else
{
	redirect("/");
}
?>