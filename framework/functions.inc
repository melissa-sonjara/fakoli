<?php
/**************************************************************

 Copyright (c) 2007,2008 Sonjara, Inc

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

/*
 * Library of commonly used functions.
 * 
 * @package Sonjara-Tools
 */

function checkRole($role)
{
	global $user;
	
	if ($role == "" || $user->role == "super") return true;
	if (!$user) return false;
	
	if ($role != "" && $user->role == "") return false;
	
	$expr = "/\\b{$user->role}\\b/";
	$allowed = (preg_match($expr, $role) > 0) ? true : false;
	
	return $allowed;
}

/**
 * MIME types allowable for image uploads 
 */
$image_types = array( "image/jpeg", "image/gif", "image/x-png", "image/pjpeg" );

/**
 * MIME types allowable for document uploads 
 */
$doc_types = array( "application/msword", 
		    "application/vnd.ms-excel", 
		    "application/excel",
		    "application/ms-excel",
		    "application/pdf",
		    "application/vnd.ms-powerpoint",
		    "application/ms-powerpoint",
		    "application/mspowerpoint",
		    "text/html",
		    "text/rtf" );

/**
 * Mapping of common file extensions to human-readable document type
 */
$doc_extensions = array( 
        ".doc" => "Microsoft Word Document",
        ".xls" => "Microsoft Excel Spreadsheet",
        ".ppt" => "Microsoft Powerpoint Presentation",
        ".pdf" => "Adobe Acrobat PDF",
        ".rtf" => "Rich Text Format",
        ".htm" => "HTML File",
        ".zip" => "ZIP Archive",
        ".jpg" => "JPEG Image File",
        ".gif" => "GIF Image File",
        ".png" => "PNG Image File"
        );

$doc_mime_types = array(
		".doc" => "application/msword",
        ".xls" => "application/vnd.ms-excel",
        ".ppt" => "application/vnd.ms-powerpoint",
        ".pdf" => "application/pdf",
        ".rtf" => "text/rtf",
        ".htm" => "text/html",
        ".zip" => "application/zip",
        ".jpg" => "image/jpeg",
        ".gif" => "image/gif",
        ".png" => "image/x-png"
);

/**
 * LEGACY Global function to connect to the database.
 * Uses the parameters defined in the global $conn array. 
 */
function connect_db()
{
	global $usr;
  	global $db;
  	global $pwd;
  	global $host;

  	/* Connecting, selecting database */
  	$link = mysql_connect($host, $usr, $pwd)
   		or die("Could not connect to database");
 	mysql_select_db($db, $link) or die("Could not select database");
  
	return $link;
}

/**
 *  Utility function to nicely format addresses, etc.
 * (skips blank entries, places all other entries on separate lines) 
 */
function formatAddress() 
{
    $numargs = func_num_args();
    $arg_list = func_get_args();
    for ($i = 0; $i < $numargs; $i++) 
    {
		if ($arg_list[$i]) echo $arg_list[$i]."<br>\n";
    }
} 


/**
 * Utility function to format currency using $ and , and stuff
 */
function formatCurrency($amount)
{
	return "$".number_format($amount, 2, '.', ',');
}

/**
 * Utility function to format date (no time component).
 * 
 * @param string $date the date to format
 */
function formatDate($date)
{
	
	if ($date !='Jan 1 1900 12:00AM')
	{
		return date("F d, Y", strtotime($date));
	}	
}
	
function formatMonthYear($date)
{
	
	if ($date !='Jan 1 1900 12:00AM')
	{
		return date("F Y", strtotime($date));
	}	
}
	
/**
 * Utility function to format date in short form (MM/DD/YYYY), with no time component.
 */
function formatDateShort($date)
{
	$text = "";
	
	if ($date)
	{
		$text = date("m/d/Y", strtotime($date));
	}
	
	if ($text == "01/01/1900") $text = "";
	 
	return $text;
}

function formatDateLong($date)
{
	$text = "";
	
	if ($date)
	{
		$text = date("l F j, Y", strtotime($date));
	}
	
	return $text;
}

/**
 * Utility function to format readable date from a timestamp (no time component).
 * Timestamp is a string in the format 'YYYYMMDDHHIISS'.
 * 
 * @param string $ts the timestamp to format. 
 */
function formatTimestamp($ts)
{
		if (!$ts) return "N/A";

		
	$yyyy = substr($ts,0,4);
	$mm   = substr($ts,4,2);
	$dd   = substr($ts,6,2);
	$hh   = substr($ts,8,2);
	$mi   = substr($ts,10,2);
	$ss   = substr($ts,12,2);

    if ($yyyy == '0000') return "N/A";
		    
	return date("F d, Y", mktime($hh,$mi,$ss,$mm,$dd,$yyyy));
}

/**
 * Returns the number of days in the specified month
 * @param number $m the month number
 * @param number $y the year
 * @return number the number of days in that month
 */
function getMonthDays($m, $y)
{
	$monthDays = array(31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31);
	$d = $monthDays[$m - 1];
	if ($m == 2 && ($y % 4) == 0 && ($y % 100) != 0) $d++;
	
	return $d;
}
	
/**
 * Format a time in 12 hour clock, no seconds
 */
function formatTime12($time)
{
	return date("g:ia", $time);
}

/**
 * Returns the current time and date in a database compatible format
 * @return string
 */
function now()
{
	return date('Y/m/d H:i:s');
}

/**
 * Utility function to escape a string correctly for use in a Javascript client-side call.
 * 
 * @param string $str the string to make safe.
 */
function jsSafe($str, $escapeEntities = false)
{
	$str = str_replace("\\", "\\\\", $str);
	$str = preg_replace("/'/","\'",$str);
	$str = preg_replace('/"/','\"',$str);
	$str = str_replace("\r", "", $str);
	$str = str_replace("\n", "\\n", $str);
	$str = str_replace("&#39;", "\'", $str);
	
	if ($escapeEntities)
	{
		$str = str_replace("&", "&amp;", $str);
	}
	return $str;
}

function htmlsafe($str)
{
	return htmlspecialchars($str, ENT_QUOTES, 'UTF-8');
}
/**
 * Outputs the MIME type list based on an array of MIME types.
 * Use this function to generate the contents of ACCEPT= attributes
 * for <INPUT TYPE="file" ...> tags.                                
 */
function printMIMETypeList($types)
{
  print "*";
  //foreach ($types as $t)
  //{
  //  if ($out) $out .= ",";
  //  $out .= $t;
  //}

  //print $out;
}

/**
 *  Uploads a file, stores the URL in the database.
 *  @param string  $input     the name of the <INPUT> tag for the file upload
 *  @param array   $types     the list of acceptable MIME types
 *  @param string  $table     the name of the database table associated with this file
 *  @param string  $field     the name of the field in the table
 *  @param string  $idfield   the name of the primary key field for the table
 *  @param integer $id        the primary key value for the row the upload is associated with
 *  @param string  $mimefield the field in the table that will store the MIME type of the file. 
 */
function uploadFile($input, $types, $table, $field, $idfield, $id, $mimefield)
{
	global $HTTP_POST_FILES;
	global $config;

	if (!$HTTP_POST_FILES[$input]) return;
	if ($HTTP_POST_FILES[$input]["name"]=="") return;

	$fileType = $HTTP_POST_FILES[$input]["type"];

	$matchingType = "";
	foreach($types as $t)
	{
		if (is_integer(strpos($fileType, $t)))
    	{
			$matchingType = $t;
      		break;
		}
	}

	if ($matchingType == "") die("No matching MIME type found for ".$fileType);

  	/* Copy across the uploaded file */

	$dir = $config["uploaddir"]."/$id";
	$name = $HTTP_POST_FILES[$input]["name"];
	$file = "$dir/$name";
	
	if (!file_exists("$dir"))
	{
		// If the directory does not exist, create it 
		mkdir("$dir");
	}
	else if (file_exists("$file"))
	{
		// If a previous copy of the file already exists, remove it
		unlink("$file");
	}
	
  	move_uploaded_file($HTTP_POST_FILES[$input]["tmp_name"], $file);
  	chmod($file, 0755);
 
 	$query = "update $table set $field='$file', $mimefield='$matchingType' where $idfield=$id";
 	
 	mysql_query($query) or die("Failed to update $table - $query");
}

/**
 * Returns a human-readable type name for a document, 
 * based on the file extension of the supplied file name.
 * 
 * @param string $doc the filename of the document
 */
function getDocType($doc)
{
    global $doc_extensions;
    
    $d = strtolower($doc);
    
    $ext = substr($d, strrpos($d, "."));
    
    $doc_type = $doc_extensions[$ext];
    
    if (!$doc_type) $doc_type = "Unknown";
    
    return $doc_type;
    
}

/**
 * Returns the file size in Kb for a document
 * 
 * @param string $doc the (local) URL of the document  
 */
function getDocSize($doc)
{
    global $config;
    
    // Find the file name for the file on disk
    
    $doc_file = $doc;
    
    $doc_file = str_replace($config["uploadurl"], $config["uploaddir"], $doc_file);
    
    // If the file exists, return its size
    
    if (file_exists($doc_file))
    {
        $doc_size = (int)(filesize($doc_file)/1024);
    }
    else
    {
        $doc_size = 0;
    }
    
    return $doc_size;
}

function getMimeType($name)
{
	global $doc_mime_types;
	
	$n = strtolower($name);
	$extension = substr($n, strrpos($n, "."));
	$type = $doc_mime_types[$extension];
	if (!$type) $type="application/octet-stream";
	return $type;
}

/**
 * Counts the number of rows in the specified table.
 * Constraints can be passed in as part of the table string if required
 * 
 * @param string $table the table to count
 */
function countRows($table, $fn="COUNT(1)")
{
	$sql = "SELECT $fn FROM $table";
	$result = mysql_query($sql) or die("countRows failed - $sql");
	
	$count = 0;
	
	if ($line = mysql_fetch_row($result))
	{
		$count = $line[0];
	}
	
	mysql_free_result($result);
	
	return $count;
}


/**
 * Security helper function. Check that the given parameter is a numeric value.
 */
function checkNumeric($p)
{
	if (is_array($p))
	{
		foreach($p as $v)
		{
			if ($v="" && !is_numeric($v)) die("Invalid Parameter");
		}
	}
	else if ($p!="" && !is_numeric($p)) die("Invalid Parameter");
	return $p;
}

/**
 * Utility function to nicely format addresses, etc.                 
 * (skips blank entries, places all other entries on separate lines) 
 *
 * @param integer $count the index of the current line
 */
function color($count)
{
	
$color = "#FFFFFF";
if ($count%2==0) $color = "#EEEEEE";
return $color;
}

function ellipsis($txt, $max, $wholeWord = false)
{
	if (strlen($txt) > $max)
	{
		if (!$wholeWord)
		{
			$txt = substr($txt, 0, $max)."...";
		}
		else
		{
			$txt = substr($txt, 0, $max);
			$pos = strrpos($txt, " ");
			if ($pos)
			{
				while($pos && $txt[$pos - 1] == '.')
				{
					$pos--;
				}
			}
			$txt = substr($txt, 0, $pos)."...";
		}
	}
	
	return $txt;
}

function rteSafe($strText) {
    //returns safe code for preloading in the RTE
    $tmpString = $strText;
    
    //convert all types of single quotes
    $tmpString = str_replace(chr(145), chr(39), $tmpString);
    $tmpString = str_replace(chr(146), chr(39), $tmpString);
    $tmpString = str_replace("'", "&#39;", $tmpString);
    
    //convert all types of double quotes
    $tmpString = str_replace(chr(147), chr(34), $tmpString);
    $tmpString = str_replace(chr(148), chr(34), $tmpString);
//  $tmpString = str_replace("\"", "\"", $tmpString);
    
    //replace carriage returns & line feeds
    $tmpString = str_replace(chr(10), " ", $tmpString);
    $tmpString = str_replace(chr(13), " ", $tmpString);
    
    return $tmpString;
}

function rtePrepare($strText, $css)
{
    global $_SERVER;    
    return rteSafe($strText);
}

function rteClean($strText)
{
	$patterns = array(	"|<\\!\\-\\-\\s+HEADERS\\s+\\-\\->.*<\\!\\-\\-\\s+/HEADERS\\s+\\-\\->|i",
						"/_base_href=\\\?\".*?\"/i",
						"|href=\\\?\"http://www.sonjara.com/ictinedtoolkit|i",
						"|src=\\\?\"http://www.sonjara.com\\/ictinedtoolkit|i",
						"|href=\\\?\"http://www.ictinedtoolkit.org|i",
						"|src=\\\?\"http://www.ictinedtoolkit.org|i");
						
	$subs = array( "", "", "href=\\\"..", "src=\\\"..", "href=\\\"..", "src=\\\"..");
						
    $strText = preg_replace($patterns, $subs, $strText);
    
    return $strText; 
}

/**
 * Returns the first sentence of the supplied text. HTML formatting is removed. The first sentence
 * is defined as the text up to the first occurrence of the substring ". ". If this substring does
 * not appear, the whole (HTML-stripped) text is returned. 
 * 
 * @param string $text the text of which the first sentence is to be returned 
 */
function firstSentence($text)
{
	$text = preg_replace("/<br>/i", " ", $text);
	$text = preg_replace("/<.*?>/", "", $text);
	$pos = strpos($text, ". ");
	return ($pos) ? substr($text, 0, $pos + 1) : $text;
}

function stripHTML($text)
{
		$text = preg_replace("/<.*?>/", "", $text);
		return $text;
}

/**
 * Runs a query, returns the first field in each row of the resultset as a comma-delimited list.
 * 
 * @param string $query the query to run.
 */
function queryList($query)
{
	$result = mssql_query($query) or die("Query Failed");
	
	$first = true;
	$text = "";
	
	while ($row = mssql_fetch_row($result))
	{
		if (!$first)
		{
			$text .= ", ";
		}
		
		$text .= $row[0];
		$first = false;
	}
	
	mssql_free_result($result);
	
	return $text; 	
}

/**
 * Parse out multiple values from the provided collection that have the same
 * root name, and return the results in an array.
 * 
 * @param array $collection the parameter collection (usually $_POST or $_GET)
 * @param string $param the root name for the params to search for
 */
function parseMultiParam($collection, $param)
{
	$len = strlen($param);
	$values = array();
	
	foreach($collection as $field => $value)
	{
		if (!strncmp($field, $param, $len))
		{
			$values[] = $value;
		}
	}
	
	return $values;
}

/**
 * Write out an option tag, marking as selected if applicable.
 * @param mixed $value the value of the option
 * @param string $text the text for the option
 * @param mixed $sel a value to compare to the $value field, 
 * to determine whether the option should be selected. 
 */
function option($value, $text, $sel = "")
{
	echo "<option value='$value'";
	if ($sel == $value)
	{
		echo " selected";
	}
	echo ">$text</option>";	
}
	
/**
 * Simplified redirect.
 * @param $page the page to redirect to
 */
function redirect($page)
{
	global $_SERVER;
	global $config;
  
	if (!$config['prettyURLs'])
	{
		$page = preg_replace("/^\\/?([\\w\\d_]+?)\\?/", "/page.php?identifier=$1&", $page);
		$page = preg_replace("/^\\/?([\\w\\d_]+)$/", "/page.php?identifier=$1", $page);
	}
	
	$server = $config['http_host'];
	if (!$server) $server="localhost:8080";
	
	if (!strncmp($page, "http://", 7))
	{
		$location = $page;
	}
	else if ($page[0] == '/')
	{
		$location = "http://$server$page";
	}
	else
	{
		$directory = dirname($_SERVER['PHP_SELF']);
		if ($directory == "/") $directory = "";
		$location = "http://".$server.$directory."/".$page;
	}
		
  	header("Location: $location");
  	exit;
}

/**
 * Send output to the trace log
 * @param $msg the message to output
 * @param $lvl the severity level (0-5) of this message
 */
function trace($msg, $lvl)
{
	global $config;
	
	if ($lvl <= $config["trace_level"])
	{
		switch($config["trace_detail"])
		{
		case TraceDetailFull:
			
			$callStack = debug_backtrace();
			if (count($callStack) > 1)
			{
				$callingFn = $callStack[1];
			}
			else
			{
				$callingFn = $callStack[0];
			}
			$file = str_replace($config["homedir"], "", $callingFn["file"]);
			$ref = $file.":".$callingFn["line"]." ";
			if ($callingFn["class"])
			{
				$ref .= $callingFn["class"].$callingFn["type"];
			}
			
			$ref .= $callingFn["function"]."(): ";
			
			error_log(date("Y-m-d H:i:s")." : ".$_SERVER['REQUEST_URI']." : ".$ref." ".$msg."\r\n", 3, $config["trace_path"]);
			break;

		case TraceDetailHigh:
			$callStack = debug_backtrace();
					if (count($callStack) > 1)
			{
				$callingFn = $callStack[1];
			}
			else
			{
				$callingFn = $callStack[0];
			}			$file = str_replace($config["homedir"], "", $callingFn["file"]);
			$ref = $file.":".$callingFn["line"]." ";
			if ($callingFn["class"])
			{
				$ref .= $callingFn["class"].$callingFn["type"];
			}
			
			$ref .= $callingFn["function"]."(): ";
			
			error_log(date("Y-m-d H:i:s")." : ".$ref." ".$msg."\r\n", 3, $config["trace_path"]);
			break;
			
				
		case TraceDetailMedium:
			error_log(date("Y-m-d H:i:s : ").$msg."\r\n", 3, $config["trace_path"]);
			break;

		case TraceDetailNormal:
		default:
			
			error_log($msg."\r\n", 3, $config["trace_path"]);
			break;
		}
	}
}


/**
 * Get human-readable file size from raw number of bytes
 * @param $size the size of the file in bytes
 * @return string human-readable file size in appropriate units
 */	
function getScaledSize($size)
{
	$result = "";
	if ($size > 1048576)
	{
		$result = sprintf("%.1fMB", ($size / 1048576));
	}
	else if ($size > 1024)
	{
		$result = intval($size / 1024)."KB";
	}
	else
	{
		$result = "$size bytes";
	}
	
	return $result;
}


$_icons = array(".doc"		=>	"/images/msword_icon.png",
				".xls"		=>	"/images/msexcel_icon.png",
				".pdf"		=>	"/images/pdf_icon.png",
				".ppt"		=>	"/images/ppt_icon.png",
				"default"	=>	"/images/file_icon.png");

/**
 * Retrieves a graphic icon appropriate for the given file
 * @param $file the name of the file
 * @return string URL of the graphic icon for the file
 */
function getIcon($file)
{
	global $_icons;
	global $auto_form_defaults;
	
	$ext = substr($file, strrpos($file, "."));
	$icon = $_icons[$ext];
	trace("getIcon($file) $ext $icon", 3);
	
	if (!isset($icon))
	{
		$icon = $_icons["default"];
	}
	
	return $auto_form_defaults["componentPath"].$icon;			
}

/**
 * Includes the contents of the given remote file, with any relative
 * src or href references converted to absolute references. The included
 * file is echoed directly to the output stream.
 * @param $host the host to retrieve the file from
 * @param $file the URI of the file on the host
 */
function includeRemote($host, $file)
{
	$ch = curl_init("http://$host/$file");
	curl_setopt($ch, CURLOPT_HEADER, 0);
	curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
	$html = curl_exec($ch);
	curl_close($ch);
	
	$html = preg_replace("/(src|href)(=['\"])\//", "$1$2http://$host/", $html);
	echo $html;
}

/**
 * Retrieve the contents of the remote file at the given URL
 * @param $url the URL to retrieve
 * @return string the contents of the file at that URL
 */
function getRemote($url)
{
	$url = canonicalizeURL($url);
	trace("Retrieving $url", 3);
	$ch = curl_init("$url");
	curl_setopt($ch, CURLOPT_HEADER, 0);
	curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
	$html = curl_exec($ch);
	curl_close($ch);
	return $html;
}

/**
 * Takes a URL and converts relative URLs to absolute URLs for the current site.
 * @param string $url the URL to convert
 * @return string full canonical form for the URL
 */
function canonicalizeURL($url)
{
	global $config;
	
	if (startsWith($url, "http://")) return $url;
	

	if (!startsWith($url, "/"))
	{
		$out = realpath($config['homedir']."/".$url);
	}
	else
	{
		$out = realpath($config['homedir'].$url);
	}
	
	if (!startsWith($out, $config['homedir'])) return null;
	$out = str_replace($config['homedir'], "http://{$config['http_host']}", $out);
	
	trace("$url -> $out", 4);
	return $out;
}

/**
 * Make a POST request to a remote URL, returning the results.
 * @param $url the URL of the page to submit the POST request to
 * @param $data the POST parameters, passed as an array of key => value pairs 
 * @param $timeout the amount of time to wait for the POST to complete
 * @return string the results of the POST request
 */
function postRemote($url, $data, $timeout = 30)
{
	$ch = curl_init();
	curl_setopt($ch, CURLOPT_URL, $url);
	curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
	curl_setopt($ch, CURLOPT_HEADER, 0);
	curl_setopt($ch, CURLOPT_POST, 1);
	curl_setopt($ch, CURLOPT_POSTFIELDS, $data);
	curl_setopt($ch, CURLOPT_CONNECTTIMEOUT, $timeout);
	$result = trim(curl_exec($ch));
	curl_close($ch);
	return $result;
}

/**
 * Retrieve the HTTP headers for the specified remote URL (excluding the body).
 * @param string $url the URL to retrieve
 * @return the headers.
 */
function getRemoteHeaders($url)
{
	$ch = curl_init();
	curl_setopt($ch, CURLOPT_HEADER, 1);
	curl_setopt($ch, CURLOPT_NOBODY, 1);
	curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
	
	$html = curl_exec($ch);
	
	curl_close($ch);
	
	return $html;
}

/**
 * Break a URL at a given string position. This can help prevent long URLs from
 * messing up your page layout.
 * @param $url the URL to break
 * @param $width the maximum line length (in characters)
 * @return string the URL, wrapped for display
 */
function wrapURL($url, $width = 120)
{
	$text = str_replace(" ", "%20", $url);
	$text = str_replace("/", " ", $text);
	$text = wordwrap($text, $width, "/<br>", true);
	$text = str_replace(" ", "/", $text);
	
	return $text;
}

/**
 * Takes a text string and converts it into a code-compliant format, suitable
 * for use as a variable name or identifier. Principally this involved replacing
 * all whitespace and sequences of non-word characters (such as punctuation) with
 * underscores.
 * @param $name the name to codify
 * @return the code-ish version of the name
 */
function codify($name)
{
	$name = preg_replace("/[\\s\\W]+/", "_", $name);
	$name = str_replace("&", "and", $name);
	return $name;
}

/**
 * Takes a variable or field name and converts it into a human-readable version
 * (assuming that the original contains human-readable words, of course). It handles
 * CamelCase and underscores. Individual words in the output are capitalized.
 * @param $name the string to prettify
 * @return string the prettified version of the string
 */
function prettify($name)
{
	$name = preg_replace("/([a-z])([A-Z0-9])/", "$1 $2", $name);
	$name = str_replace("_", " ", $name);
	$name = ucwords($name);
	
	return $name;
}

/**
 * Takes a singular string and makes it plural.
 * @param $text the string to make plural
 * @return string the plural version
 */
function pluralize($text)
{
	$c = $text;

	if (preg_match("/[^aeiouAEIOU]y$/", $c))
	{
		$c = substr($c, 0, -1)."ies";
	}
	else if (preg_match("/(?:ss|o|x)$/", $c))
	{
		$c .= "es";
	}
	else
	{
		$c .= "s";
	}

	return $c;
}

/**
 * Takes a string and formats it for display as HTML, removing any 
 * HTML tags it contains, and converting carriage returns to br tags.
 * @param $text the text to format
 * @return string the formatted text
 */
function formatAsHTML($text)
{
	if (preg_match("/<.*?>/", $text)) return $text;
	$text = str_replace("\n", "<br/>", $text);
	
	return $text;
}

/**
 * Tests whether a string starts with a given sub-string
 * @param $text the string to test
 * @param $start the sub-string to look for at the start
 * @return boolean true if the string starts with the sub-string, false otherwise
 */
function startsWith($text, $start)
{
	return (strncmp($text, $start, strlen($start)) == 0);
}

/**
 * Tests whether a string ends with the given sub-string
 * @param $text the string to test
 * @param $end the sub-string to look for at the end
 * @return boolean true if the string ends with the sub-string, false otherwise
 */
function endsWith($text, $end)
{
	return (substr($text, -strlen($end)) == $end);
}

/**
 * Removes all HTML tags from the specified string
 * @param $text the string from which tags are to be removed
 * @return string the cleaned text.
 */
function stripHTMLTags($text)
{
	return preg_replace("/<.*?>/", "", $text);
}

/**
 * Adds the english ordinal suffix to a number. If the value passed is not purely numeric,
 * it will be returned unchanged.
 * @param $n the value for the ordinal suffix
 * @param $sup true to add HTML superscript tag, false for plain text
 * @return string the value with the appropriate ordinal suffix appended.
 */
function ordinalSuffix($n, $sup = false)
{
	$suffix = "";
	if (is_numeric($n))
	{
		$n = trim($n);
		
		$lastTwo = substr($n, -2, 2);
		$lastOne = substr($n, -1, 1);
		
		if ($lastTwo == "11" || $lastTwo == "12" || $lastTwo == "13") $suffix = "th";
		else 
		{
			switch($lastOne)
			{
				case "1": $suffix = "st"; break;
				case "2": $suffix = "nd"; break;
				case "3": $suffix = "rd"; break;
				default: $suffix = "th";
			}
		}
	}
	else return $n;
	
	if ($sup) return ($sup) ? "$n<sup>$suffix</sup>" : "$n$suffix";
}

function makeURL($uri)
{
	 if (!startsWith($uri, "http://")) $uri = "http://$uri";
	 return $uri;
}

function appendToQueryString($qs, $params)
{
	if (!$qs) 
	{
		$qs = "?$params";
	}
	else
	{
		$qs .= "&$params";
	}
	
	if ($qs[0] != "?") $qs = "?$qs";
	$qs = preg_replace("/&&+/", "&", $qs);
	
	return $qs;
}

function makeRandomString($len)
{
	$str = "";
	for($i = 0; $i < $len; ++$i)
	{
		$str .= chr(ord('a') + rand(0, 25));
	}
	
	return $str;
}
?>