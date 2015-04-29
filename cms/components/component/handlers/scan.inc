<?php
/**************************************************************

 Copyright (c) 2010 Sonjara, Inc

 Permission is hereby granted, free of charge, to any person
 obtaining a copy of this software and associated documentation
 files (the "Software"), to deal in the Software without
 restriction, including without limitation the rights to use,
 copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following
 conditions:

 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.

 Except as contained in this notice, the name(s) of the above 
 copyright holders shall not be used in advertising or otherwise 
 to promote the sale, use or other dealings in this Software 
 without prior written authorization.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE.

*****************************************************************/

if (!checkRole("admin") && $_SERVER["REMOTE_ADDR"] != $_SERVER["SERVER_ADDR"] && $_SERVER["REMOTE_ADDR"] != "127.0.0.1")
{
	throw new FakoliException("Anonymous component scans cannot be run from {$_SERVER["REMOTE_ADDR"]}");
}

if ($_SERVER["REMOTE_ADDR"] == "127.0.0.1" && array_key_exists("clear", $_GET))
{
	$db = ConnectionManager::getConnection();
	$db->exec("TRUNCATE TABLE component");

	Cache::invalidate("fakoli_components");
}

$verbosity = checkNumeric($_GET["verbosity"]);
if ($verbosity) ComponentManager::$verbosityLevel = $verbosity;
$plain = checkNumeric($_GET["plain"]);
if ($plain) ComponentManager::$plainOutput = true;

Fakoli::$rollbackBuffers = false;

try
{
	$mgr = new ComponentManager();
	$mgr->scan();
}
catch(Exception $e)
{
	if (checkRole("admin"))
	{
		throw $e;
	}
	else
	{
		echo $e->toString();
	}
}

if (checkRole("admin"))
{
	redirect("/admin/components");
}
else
{
	echo "Scan Complete\n";
}

?>