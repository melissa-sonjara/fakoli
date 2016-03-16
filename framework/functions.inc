<?php
/**************************************************************

 Copyright (c) 2007-2010 Sonjara, Inc

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

require_once realpath(dirname(__FILE__)."/document_handler.inc");
require_once realpath(dirname(__FILE__)."/date_utils.inc");

/**
 * @package Fakoli Framework
 * @file
 * @addtogroup functions Utility Functions
 * Library of commonly used functions.
 * @{
 */

/**
 * Check whether the user has one of a specified set of roles
 * @param string $role the role(s) to check against, as a comma-delimited list
 * @param object $account (optional) the account object to check (must have a 'role' property). 
 * If not provided, the default user account is used (assumed to be in the global $user variable).
 * @return boolean true if the user has one of the roles, false otherwise.
 */
function checkRole($role, $account = null)
{
	global $user;
	
	if (!$account) $account = $user;
	
	if ($role == "" || $account->role == "super" || $account->role == "admin") return true;
	if (!$account) return false;
	
	if ($role != "" && $account->role == "") return false;
	
	$userRoles = explode(",", $account->role);
	foreach($userRoles as $ur)
	{
		if ($ur == "super" || $ur == "admin") return true;
		$expr = "/\\b{$ur}\\b/";
		$allowed = (preg_match($expr, $role) > 0) ? true : false;
		if ($allowed) break;
	}
	
	return $allowed;
}

/**
 * Determine whether a user has a specific role explicitly set.
 * Unlike checkRole() this does not always return true if a user is super or admin. 
 * @param string $role
 * @param object $account
 * @return boolean true if the user has the specified role explicitly set, false otherwise.
 */
function hasRole($role, $account = null)
{
	global $user;
	
	if (!$account) $account = $user;
	if (!$role) return false;

	$expr = "/\\b{$role}\\b/";
	return (preg_match($expr, $account->role) > 0) ? true : false;
}

/**
 * Filter the given list, retaining only the objects that match the current
 * user's role profile in the given parameter.
 * 
 * @param string $items the array of items to be filtered
 * @param string $field the name of the field defining the role restriction
 * @return array the filtered array
 */
function filterByRole($items, $field)
{
	$ret = array();
	foreach($items as $item)
	{
		if (checkRole($item->get($field)))
		{
			$ret[] = $item;
		}
	}
	
	return $ret;
}

/**
 * MIME types allowable for image uploads 
 */
$image_types = array( "image/jpeg", "image/gif", "image/x-png", "image/pjpeg" );
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
 * 
 * Deprecated
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
 * 
 * Deprecated
 */
function formatCurrency($amount)
{
	return "$".number_format($amount, 2, '.', ',');
}

/**
 * Utility function to format date (no time component).
 * 
 * @param string $date the date to format
 * 
 * Deprecated
 */
function formatDate($date)
{
	
	if ($date !='Jan 1 1900 12:00AM')
	{
		return date("F d, Y", strtotime($date));
	}	
}
	
/**
 * 
 * @param $date
 * 
 * Deprecated
 */
function formatMonthYear($date)
{
	
	if ($date !='Jan 1 1900 12:00AM')
	{
		return date("F Y", strtotime($date));
	}	
}
	
/**
 * Utility function to format date in short form (MM/DD/YYYY), with no time component.
 * 
 * Deprecated
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

/**
 * 
 * @param $date
 * 
 * Deprecated
 */
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
 * 
 * Deprecated
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

/*
 * Format timestamp with date in short format
 * e.g., 1/2/2011 and include time in 12 hour format
 *  e.g., 10:15am
 *  
 *  Deprecated
 */
function formatTimestampShort($ts)
{
		if (!$ts) return "N/A";

	$yyyy = substr($ts,0,4);
	$mm   = substr($ts,4,2);
	$dd   = substr($ts,6,2);
	$hh   = substr($ts,8,2);
	$mi   = substr($ts,10,2);
	$ss   = substr($ts,12,2);

    if ($yyyy == '0000') return "N/A";
		    
	return date("m/d/Y g:ia", mktime($hh,$mi,$ss,$mm,$dd,$yyyy));
	
}

/**
 * Returns the number of days in the specified month
 * @param number $m the month number
 * @param number $y the year
 * @return number the number of days in that month
 * 
 * Deprecated
 */
function getMonthDays($m, $y)
{
	$monthDays = array(31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31);
	$d = $monthDays[$m - 1];
	if ($m == 2 && ($y % 4) == 0 && ($y % 100) != 0) $d++;
	
	return $d;
}

/**
 * Get the number of week days in the specified month
 * @param number $m the month number
 * @param number $y the year
 * @return number the number of week days in that month
 * 
 * Deprecated
 */
function getMonthWeekDays($month, $year)
{
	$weekDaysInMonth = array
	(
		28	=>	array(20, 20, 20, 20, 20, 20, 20),
		29	=>	array(21, 21, 21, 21, 21, 20, 20),
		30	=>	array(22, 22, 22, 22, 21, 20, 21),
		31	=>	array(23, 23, 23, 22, 21, 21, 22)
	);
	
	$days = getMonthDays($month, $year);
	$date = strtotime("$year-$month-01");
	
	$first = date("N", $date) - 1;
	return $weekDaysInMonth[$days][$first];
}

/**
 * Format a time in 12 hour clock, no seconds
 * 
 * Deprecated
 */
function formatTime12($time)
{
	return date("g:ia", $time);
}

/**
 * Format a date and time in 12 hour clock, no seconds
 * 
 * Deprecated
 */
function formatDateTime12($time)
{
	return date("m/d/Y g:ia", $time);
}

/*
 * Formats a datetime in date short format
 * with hours on 12 hour clock, no seconds.
 * If time component is 00:00:00, then
 * don't return time.
 * 
 * Deprecated
 */
function formatDateTime12Hr($datetime)
{
	$yyyy = substr($datetime,0,4);
	$mm   = substr($datetime,5,2);
	$dd   = substr($datetime,8,2);
	$hh   = substr($datetime,11,2);
	$mi   = substr($datetime,14,2);
	$ss = "00";
	
    if ($yyyy == '0000') return "N/A";
    if($hh == '00')
		return date("m/d/Y", mktime($hh,$mi,$ss,$mm,$dd,$yyyy));
    else
		return date("m/d/Y g:ia", mktime($hh,$mi,$ss,$mm,$dd,$yyyy));
}

/*
 * If start and end dates are the same date 
 * then show the date followed by start and end time.
 * Otherwise, show start date and time to end date
 * and time. 
 * 
 * $start - datetime
 * $end - datetime
 * 
 * Deprecated
 */
function formatStartDateToEndDate($start, $end)
{
	$yyyy = substr($start,0,4);
	$yyyy_2 = substr($end,0,4);
	$mm   = substr($start,5,2);
	$mm_2   = substr($end,5,2);
	$dd   = substr($start,8,2);
	$dd_2   = substr($end,8,2);
			
	if($yyyy != $yyyy_2 || $mm != $mm_2 || $dd != $dd_2)
	{
		return formatDateTime12Hr($start) ." &ndash; ". formatDateTime12Hr($end);
	}
	else
	{
		$hh   = substr($start,11,2);
		$mi   = substr($start,14,2);
		$hh_2   = substr($end,11,2);
		$mi_2   = substr($end,14,2);
		$s_time = date("g:ia", mktime($hh,$mi));
		$e_time = date("g:ia", mktime($hh_2,$mi_2));
		return formatDateShort($start) . " $s_time &ndash; $e_time";
	}	
}

/*
 * Given a phone number containing various
 * separator characters between the numbers,
 * clear out the separators and standardize
 * the format as 
 * (xxx) xxx-xxxx
 * 
 * tylerhall on 11/30/-1
 * http://snipplr.com/view/25/format-phone-number/
 * 
 * Deprecated
 */
function format_phone($phone)
{
	$phone = preg_replace("/[^0-9]/", "", $phone);
	
	if(strlen($phone) == 7)
		return preg_replace("/([0-9]{3})([0-9]{4})/", "$1-$2", $phone);
	elseif(strlen($phone) == 10)
		return preg_replace("/([0-9]{3})([0-9]{3})([0-9]{4})/", "($1) $2-$3", $phone);
	else
		return $phone;
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
 * Returns today as a string. All of it.
 */
function today()
{
	return date('Y-m-d');
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
	$str = str_ireplace("</script>", "\\x3C/script\\x3E", $str);
	
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

function cleanHTMLTags($str)
{
	return str_replace(array("<", ">"), array("&lt;", "&gt;"), $str);
}

function HTMLToText($html)
{
	$patterns = array("/<br>/", "/&nbsp;/", "/<p>/", "/<BR>/", "/<P>/");
	$replacements = array("\r\n", " ", "\r\n", "\r\n", "\r\n");
	$text =  stripHTML(preg_replace($patterns, $replacements, $html));
	
	return $text;
}

function cleanQuotes($str)
{
	$chr_map = array(
			// Windows codepage 1252
			"\xC2\x82" => "'", // U+0082 U+201A single low-9 quotation mark
			"\xC2\x84" => '"', // U+0084 U+201E double low-9 quotation mark
			"\xC2\x8B" => "'", // U+008B U+2039 single left-pointing angle quotation mark
			"\xC2\x91" => "'", // U+0091 U+2018 left single quotation mark
			"\xC2\x92" => "'", // U+0092 U+2019 right single quotation mark
			"\xC2\x93" => '"', // U+0093 U+201C left double quotation mark
			"\xC2\x94" => '"', // U+0094 U+201D right double quotation mark
			"\xC2\x9B" => "'", // U+009B U+203A single right-pointing angle quotation mark
	
			// Regular Unicode     // U+0022 quotation mark (")
			// U+0027 apostrophe     (')
			"\xC2\xAB"     => '"', // U+00AB left-pointing double angle quotation mark
			"\xC2\xBB"     => '"', // U+00BB right-pointing double angle quotation mark
			"\xE2\x80\x98" => "'", // U+2018 left single quotation mark
			"\xE2\x80\x99" => "'", // U+2019 right single quotation mark
			"\xE2\x80\x9A" => "'", // U+201A single low-9 quotation mark
			"\xE2\x80\x9B" => "'", // U+201B single high-reversed-9 quotation mark
			"\xE2\x80\x9C" => '"', // U+201C left double quotation mark
			"\xE2\x80\x9D" => '"', // U+201D right double quotation mark
			"\xE2\x80\x9E" => '"', // U+201E double low-9 quotation mark
			"\xE2\x80\x9F" => '"', // U+201F double high-reversed-9 quotation mark
			"\xE2\x80\xB9" => "'", // U+2039 single left-pointing angle quotation mark
			"\xE2\x80\xBA" => "'", // U+203A single right-pointing angle quotation mark
	);
	$chr = array_keys  ($chr_map); // but: for efficiency you should
	$rpl = array_values($chr_map); // pre-calculate these two arrays
	$str = str_replace($chr, $rpl, html_entity_decode($str, ENT_QUOTES, "UTF-8"));	
	return $str;
}

/**
 * Outputs the MIME type list based on an array of MIME types.
 * Use this function to generate the contents of ACCEPT= attributes
 * for <INPUT TYPE="file" ...> tags.    
 * 
 * Deprecated
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
 *  
 *  Deprecated
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
 * 
 * Deprecated
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
 * 
 * Deprecated
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

// ?Is getMIMEType supposed to be used instead of getMimeType??
// Doesn't seem to work. 
/**
 * 
 * @param $name
 * 
 * Deprecated
 */
function getDocMimeType($name)
{
	global $doc_mime_types;
	
	$n = strtolower($name);
	$extension = substr($n, strrpos($n, "."));
	$type = $doc_mime_types[$extension];
	if (!$type) $type="application/octet-stream";
	return $type;
	
}

/*function getMimeType($name)
{
	global $doc_mime_types;
	
	$n = strtolower($name);
	$extension = substr($n, strrpos($n, "."));
	$type = $doc_mime_types[$extension];
	if (!$type) $type="application/octet-stream";
	return $type;
}*/

/**
 * 
 * @param unknown_type $doc
 * 
 * Deprecated
 */
function getDocIcon($doc)
{
	global $doc_icons;
	$n = strtolower($doc);
	$extension = substr($n, strrpos($n, "."));
	$icon = $doc_icons[$extension];
	if (!$icon) $icon = "/fakoli/images/file_icon.png";
	return $icon;
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
			if ($v!="" && !is_numeric($v)) die("Invalid Parameter");
		}
	}
	else if ($p!="" && !is_numeric($p)) die("Invalid Parameter");
	return $p;
}


/**
 * Security helper function. Check that the given parameter is a valid identifier
 * (alphanumeric, _ and digits only with no spaces). Empty string is valid
 */
function checkIdentifier($p)
{
	return preg_match("/^[A-Za-z0-9_\\-]*$/", $p) ? $p : die("Invalid Parameter");
}

/**
 * Utility function to nicely format addresses, etc.                 
 * (skips blank entries, places all other entries on separate lines) 
 *
 * @param integer $count the index of the current line
 * 
 * Deprecated
 */
function color($count)
{
	
$color = "#FFFFFF";
if ($count%2==0) $color = "#EEEEEE";
return $color;
}

/**
 * Truncate the supplied text at the given maximum length. If the
 * string is truncated then an ellipsis is added.
 * 
 * @param unknown_type $txt the text to truncate
 * @param unknown_type $max the maximum allowed length
 * @param unknown_type $wholeWord true to truncate to whole words only 
 */
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

/**
 * 
 * @param unknown_type $strText
 * 
 * Deprecated
 */
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

/**
 * 
 * @param unknown_type $strText
 * @param unknown_type $css
 * 
 * Deprecated
 */
function rtePrepare($strText, $css)
{
    global $_SERVER;    
    return rteSafe($strText);
}

/**
 * 
 * @param unknown_type $strText
 * 
 * Deprecated
 */
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
	/*$text = preg_replace("/<.*?>/", "", $text);*/
	$text = strip_tags($text);
	
	// JDG 5/19/2011 - remove &ndash, &nbsp;
	$text = html_entity_decode($text);
	$text = trim($text);
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
	if ($sel === $value)
	{		
		echo " selected";
	}
	echo ">$text</option>";	
}


/**
 * Write out an option tag to a string, marking as selected if applicable.
 * @param mixed $value the value of the option
 * @param string $text the text for the option
 * @param mixed $sel a value to compare to the $value field, 
 * to determine whether the option should be selected. 
 */
function str_option($value, $text, $sel = "")
{
	$opt = "<option value='$value'";
	if ($sel == $value)
	{
		$opt .= " selected";
	}
	$opt .= ">$text</option>";

	return $opt;
}

/**
 * Simplified redirect.
 * @param $page the page to redirect to
 */
function redirect($page)
{
	global $_SERVER;
	global $config;
  
	if ($config['prettyURLs'] === false)
	{
		$page = preg_replace("/^\\/?([\\w\\d_]+?)\\?/", "/page.php?identifier=$1&", $page);
		$page = preg_replace("/^\\/?([\\w\\d_]+)$/", "/page.php?identifier=$1", $page);
	}
	
	trace("redirect:: page is $page", 3);
	$server = $config['http_host'];
	if (!$server) $server="localhost:8080";

	//AJG: 3/9/11 - maintain current protocol by default
	
	$https = $_SERVER['HTTPS'];
	if ($https == "off") $https = false;
	
	$protocol = $https ? "https" : "http";
	
	// JDG 9/27/10 - handle https
	if (!strncmp($page, "http://", 7) OR (!strncmp($page, "https://", 8)))
	{
		$location = $page;
	}
	else if ($page[0] == '/')
	{
		$location = "$protocol://$server$page";
	}
//	else if (preg_match( "/^[\\w\\d_\\-]+$/", $page))
//	{
//		$location = "http://$server/$page";
//	}
	else
	{
		$directory = str_replace(DIRECTORY_SEPARATOR, '/', dirname($_SERVER['REQUEST_URI']));
		if ($directory == "/") $directory = "";
		$location = "$protocol://".$server.$directory."/".$page;
	}
		
  	header("Location: $location");
  	session_write_close();
  	exit;
}

/**
 * Returns the query string for the current page, cleaned of any 
 * Fakoli-related navigation parameters.
 */
function getCleanQueryString()
{
	$qs = $_SERVER['QUERY_STRING'];

	$params = array();
	
	foreach($_GET as $key => $value)
	{
		if ($key != "identifier" && $key != "section") $params[$key] = $value;
	}
	
	return http_build_query($params);
}

$traceLevels = array(1	=>	"FATAL",
					 2	=>	"ERROR",
					 3	=>	"TRACE",
					 4	=>	"DEBUG",
					 5  =>  "NOTE*");	
/**
 * Send output to the trace log
 * @param $msg the message to output
 * @param $lvl the severity level (0-5) of this message
 */
function trace($msg, $lvl, $callStack = null)
{
	global $config;
	global $traceLevels;
	
	if ($lvl <= $config["trace_level"])
	{
		switch($config["trace_detail"])
		{
		case TraceDetailFull:
			
			$callStack = $callStack ? $callStack : debug_backtrace();
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
			
			error_log("[".$traceLevels[$lvl]."] ".date("Y-m-d H:i:s")." : ".$_SERVER['REQUEST_URI']." : ".$ref." ".$msg."\r\n", 3, $config["trace_path"]);
			break;

		case TraceDetailHigh:
			
			$callStack = $callStack ? $callStack : debug_backtrace();

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
			
			error_log("[".$traceLevels[$lvl]."] ".date("Y-m-d H:i:s")." : ".$ref." ".$msg."\r\n", 3, $config["trace_path"]);
			break;
			
				
		case TraceDetailMedium:
			error_log("[".$traceLevels[$lvl]."] ".date("Y-m-d H:i:s : ").$msg."\r\n", 3, $config["trace_path"]);
			break;

		case TraceDetailNormal:
		default:
			
			error_log("[".$traceLevels[$lvl]."] ".$msg."\r\n", 3, $config["trace_path"]);
			break;
		}
	}
}


/**
 * Get human-readable file size from raw number of bytes
 * @param $size the size of the file in bytes
 * @param $decimal_places the number of decimal places in the output (default is 1)
 * @return string human-readable file size in appropriate units
 */	
function getScaledSize($size, $decimal_places = 1)
{
	if (!$size) return "";
	
	$result = "";
	if ($size > 1073741824)
	{
		$result = sprintf("%.{$decimal_places}fGB", ($size / 1073741824));
	}
	else if ($size > 1048576)
	{
		$result = sprintf("%.{$decimal_places}fMB", ($size / 1048576));
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

function roundFormat($num,$abbrev = false)
{
	if (!$num) return "";
	
	$result = "";
	if ($abbrev == false) {
		if ($num >= 1000000000) {
			$result = ($num / 1000000000).toFixed(1).replace('/\.0$/', '') + ' billion';
		}
		if ($num >= 1000000) {
			$result = ($num / 1000000).toFixed(1).replace('/\.0$/', '') + ' million';
		}
		if ($num >= 1000) {
			$result = ($num / 1000).toFixed(0).replace('/\.0$/', '') + ',000';
		}
	} else {
		if ($num >= 1000000000) {
			$result = ($num / 1000000000).toFixed(1).replace('/\.0$/', '') + 'bn';
		}
		if ($num >= 1000000) {
			$result = ($num / 1000000).toFixed(1).replace('/\.0$/', '') + 'm';
		}
		if ($num >= 1000) {
			$result = ($num / 1000).toFixed(0).replace('/\.0$/', '') + ',000';
		}
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
	curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, false);
	curl_setopt($ch, CURLOPT_HEADER, 0);
	curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
	$html = curl_exec($ch);
	
	if ($html === FALSE) throw new Exception(curl_error($ch));
	
	curl_close($ch);
	
	return $html;
}

/**
 * Retreive the contents of the remote file at the given URL 
 * and save it to the specified location on the local disk.
 * 
 * @param $url the URL to retrieve
 * @param $saveAs the path and file name where the downloaded file should be stored.
 * 
 * @return returns the content type of the retrieved file as specified by the server
 */
function saveRemote($url, $saveAs)
{
	trace("Downloading $url to $saveAs", 3);
	
	$ch = curl_init ($url);
	curl_setopt($ch, CURLOPT_HEADER, 0);
	curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, false);
	curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
	curl_setopt($ch, CURLOPT_BINARYTRANSFER,1);
	curl_setopt($ch, CURLOPT_FOLLOWLOCATION, true);
	
	$contentType = curl_getinfo($ch, CURLINFO_CONTENT_TYPE);
	$bytes = curl_exec($ch);
	curl_close ($ch);
	
	trace(count($bytes)." bytes recieved", 3);
	
	if(file_exists($saveAs))
	{
		unlink($saveAs);
	}
	
	file_put_contents($saveAs, $bytes);
	
	return $contentType;
}

/**
 * Takes a URL and converts relative URLs to absolute URLs for the current site.
 * @param string $url the URL to convert
 * @return string full canonical form for the URL
 */
function canonicalizeURL($url)
{
	global $config;
	
	if (startsWith($url, "http://") || startsWith($url, "https://")) return $url;
	

	if (!startsWith($url, "/"))
	{
		$out = realpath($config['homedir']."/".$url);
	}
	else
	{
		$out = realpath($config['homedir'].$url);
	}
	
	$out = str_replace('\\', '/', $out);
	
	if (!startsWith($out, $config['homedir'])) return null;
	$out = str_replace($config['homedir'], "http://{$config['http_host']}", $out);
	
	trace("$url -> $out", 4);
	return $out;
}

/**
 * Sanitize a file path, removing relative path sections and ensuring the correct
 * directory separator is used throughout.
 * @param string $path potentially dirty path
 * @return string clean path
 */
function sanitizePath($path)
{
	$path = str_replace("/", DIRECTORY_SEPARATOR, $path);
	$path = str_replace("\\", DIRECTORY_SEPARATOR, $path);
	$path = str_replace("..".DIRECTORY_SEPARATOR, DIRECTORY_SEPARATOR, $path);
	$path = str_replace(DIRECTORY_SEPARATOR.DIRECTORY_SEPARATOR, DIRECTORY_SEPARATOR, $path);
	return $path;
}
	
/**
 * Make a POST request to a remote URL, returning the results.
 * @param $url the URL of the page to submit the POST request to
 * @param $data the POST parameters, passed as an array of key => value pairs 
 * @param $timeout the amount of time to wait for the POST to complete
 * @return string the results of the POST request
 */
function postRemote($url, $data = null, $timeout = 30)
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

function postRemoteXML($url, $xml, $timeout = 30)
{
	$ch = curl_init($url);
	curl_setopt($ch, CURLOPT_MUTE, 1);
	curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, 0);
	curl_setopt($ch, CURLOPT_POST, 1);
	curl_setopt($ch, CURLOPT_HTTPHEADER, array('Content-Type: text/xml'));
	curl_setopt($ch, CURLOPT_POSTFIELDS, $xml);
	curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
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
	curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, false);
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
 * @param $count integer (optional) the number of items, to determine whether to pluralize or not
 * @return string the plural version
 */
function pluralize($text, $count = 0)
{
	if ($count == 1) return $text;
	
	$c = $text;

	if (preg_match("/[^aeiouAEIOU]y$/", $c))
	{
		$c = substr($c, 0, -1)."ies";
	}
	else if (preg_match("/(?:ss|[^aeiou]o|x|s|ch)$/", $c))
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

	// JDG 2/10/12 - fix was not returning value if $sup false
	if ($sup) 
		return "$n<sup>$suffix</sup>"; 
	else
		return "$n$suffix";
}

/**
 * Ensures that the http:// protocol specifier prefix is present in the supplied string
 * @param string $uri the string to process
 * @return string the full URL with protocol prefix added if it was missing.
 */
function makeURL($uri)
{
	 if (!startsWith($uri, "http://")) $uri = "http://$uri";
	 return $uri;
}
 
/**
 * For dataitems that are rendered as 
 * CheckListFieldRenderer, this function converts
 * the comma-delimited list of items for display.
 * 
 * To do: add handling of grouped checkboxes and
 * provide parameter for delimiter between items
 *
 * Deprecated
 */
function formatCheckListItemsforView($options, $items)
{
	if(count($options) == 0) return;

	$values = explode(",", $items);
	
	foreach($values as $value)
	{
		if(array_key_exists($value, $options))
		{
			$list .= $options[$value] . ", ";
		}
		else
			$list .= $value . ", ";
	}
	
	if(strlen($list) > 0)
		$list = substr($list, 0, strlen($list) - 2);
	
	return $list;
}

/**
 * Returns the base URI for the current script, with the query string removed.
 * string @param $uri (optional) the URI to truncate - if not specified, the URI
 * for the currently executing script is used.
 */
function baseURI($uri = null)
{
	if (!$uri) $uri = $_SERVER['REQUEST_URI'];
	return preg_replace("/\\?.*$/", "", $uri);
}

/**
 * Appends the specified parameters to the supplied query string.
 * @param string $qs the query string
 * @param string $params parameters to append in query string format
 * @return combined string
 */
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

$__urlRewriteParams = array();

/**
 * Register any URL rewrite parameters that should be excluded
 * from query string reformatting.
 */
function registerURLRewriteParameters()
{
	global $__urlRewriteParams;
	
	for($i = 0; $i < func_num_args(); ++$i)
	{
		$__urlRewriteParams[func_get_arg($i)] = true;
	}		
}

/**
 * Generates a query string containg the full set of values
 * passed to this page both as GET and POST parameters.
 * @return string the full query string for the current page.
 */
function getFullQueryString()
{
	global $__urlRewriteParams;
	
	$qs = "";
	$sep = "?";
	
	foreach($_GET as $name => $value)
	{
		if (array_key_exists($name, $__urlRewriteParams)) continue;
		 
		if (is_array($value))
		{
			foreach($value as $key => $val)
			{
				$qs .= $sep.urlencode($name)."[".urlencode($key)."]=".urlencode($val);
				$sep = "&";
			}
		}
		else
		{
			$qs .= $sep.urlencode($name)."=".urlencode($value); 
			$sep = "&";
		}
	}
	
	foreach($_POST as $name => $value)
	{
		if (is_array($value))
		{
			foreach($value as $key => $val)
			{
				$qs .= $sep.urlencode($name)."[".urlencode($key)."]=".urlencode($val);
				$sep = "&";
			}
		}
		else
		{
			$qs .= $sep.urlencode($name)."=".urlencode($value); 
			$sep = "&";
		}
	}

	return $qs;
}
		
/*
 * Convert a php array to a javascript array.
 * 
 * @items - array of objects or simple array
 * 
 * @field - if items is array of objects, the field
 * in the object to use to create the javascript array
 * 
 * returns javascript array in the format "[cat, dog, spider]";
 */
function toJSArray($items, $field = "")
{
	$js_arr = "[";
	$first = true;

	if(count($items) == 0)
		return "";
		
	if(is_object($items[0]) && !$field)
	{
		$field = $items[0]->getPrimaryKey();
	}
		
	foreach($items as $item)
	{
		if (!$first) $js_arr .=", ";
		$js_arr .= (is_object($item) && $field) ? $item->$field : $item;
		$first = false;
	}
		
	$js_arr .= "]";
	
	return $js_arr;
}

/**
 * Creates a randomized string of characters. This is not cryptographically secure
 * and should not be used for cryptographic operations.
 * 
 * @param int $len the length of the string to generate
 * @return string the generated string
 */
function makeRandomString($len)
{
	$str = "";
	for($i = 0; $i < $len; ++$i)
	{
		$str .= chr(ord('a') + rand(0, 25));
	}
	
	return $str;
}

/**
 * Filter an array based on applying the specified callback to the keys.
 * @param array $input the array to be filtered
 * @param callback $callback a callback function or method
 * @return array the filtered array.
 */
function arrayFilterKey( $input, $callback ) 
{
    if ( !is_array( $input ) ) 
    {
        return null;
    }
   
    if ( empty( $input ) ) 
    {
        return $input;
    }
   
    $filteredKeys = array_filter( array_keys( $input ), $callback );
    
    if ( empty( $filteredKeys ) ) 
    {
        return array();
    }
   
    $input = array_intersect_key( array_flip( $filteredKeys ), $input );
   
    return $input;
}

/**
 * Returns a string output and exits the script cleanly. We used to be
 * able to use die(), but because of PHP 5's messed up session handling
 * this throws an error if there is a custom session handler registered.
 * 
 * @param string $msg
 */
function ajaxReturn($msg = null)
{
	if (class_exists(FakoliEarlyExit))
	{
		throw new FakoliEarlyExit($msg);
	}
	else
	{
		session_write_close();
		die($msg);
	}
}

/**
 * 
 * @param String $filename
 * 
 * Deprecated
 */
function getMIMEType($filename) 
{
	$mime_types = array(

		'txt' => 'text/plain',
        'htm' => 'text/html',
        'html' => 'text/html',
        'css' => 'text/css',
        'js' => 'application/javascript',
        'json' => 'application/json',
        'xml' => 'application/xml',
        'swf' => 'application/x-shockwave-flash',
        'flv' => 'video/x-flv',

        // images
        'png' => 'image/png',
        'jpe' => 'image/jpeg',
        'jpeg' => 'image/jpeg',
        'jpg' => 'image/jpeg',
        'gif' => 'image/gif',
        'bmp' => 'image/bmp',
        'ico' => 'image/vnd.microsoft.icon',
        'tiff' => 'image/tiff',
        'tif' => 'image/tiff',
        'svg' => 'image/svg+xml',
        'svgz' => 'image/svg+xml',

        // archives
        'zip' => 'application/zip',
        'rar' => 'application/x-rar-compressed',
        'exe' => 'application/x-msdownload',
        'msi' => 'application/x-msdownload',
        'cab' => 'application/vnd.ms-cab-compressed',

        // audio/video
        'mp3' => 'audio/mpeg',
        'qt' => 'video/quicktime',
        'mov' => 'video/quicktime',

        // adobe
        'pdf' => 'application/pdf',
        'psd' => 'image/vnd.adobe.photoshop',
        'ai' => 'application/postscript',
        'eps' => 'application/postscript',
        'ps' => 'application/postscript',

        // ms office
        'doc' => 'application/msword',
        'rtf' => 'application/rtf',
        'xls' => 'application/vnd.ms-excel',
        'ppt' => 'application/vnd.ms-powerpoint',

        // open office
        'odt' => 'application/vnd.oasis.opendocument.text',
        'ods' => 'application/vnd.oasis.opendocument.spreadsheet',
    );

    $ext = strtolower(array_pop(explode('.',$filename)));

    if (array_key_exists($ext, $mime_types)) 
    {
        return $mime_types[$ext];
    }
    elseif (function_exists('finfo_open')) 
    {
        $finfo = finfo_open(FILEINFO_MIME);
        $mimetype = finfo_file($finfo, $filename);
        finfo_close($finfo);
        return $mimetype;
    }
    else 
    {
    	// Prevent other file types (such as source code or configuration files) from being served
        die("");
    }
}

/**
 * Remove specified keys from the given array
 * @param array $array the array to strip
 * @return stripped array
 */
function array_remove_keys($array)
{
	for($i = 1; $i < func_num_args(); ++$i)
	{
		unset($array[func_get_arg($i)]);
	}
	
	return $array;
}

/**
 * Function to provide html to XML entity renaming.
 * Provides fallback for PHP versions earlier than 5.4 that
 * do not have the ENT_XML1 format available for the htmlentities() call.
 * 
 * @param string $string the string to convert
 * @return string
 */
function xmlEntities($string)
{
	if (defined(ENT_XML1)) return htmlentities($value, ENT_XML1);

	$result = '';
	foreach (str_split(utf8_decode(htmlspecialchars($string))) as $char)
	{
		$num = ord($char);
		if ($num > 127) {
			$result .= '&#' . $num . ';';
		} else {
			$result .= $char;
		}
	}
	return $result;
}

/**
 * Encode text to 7-bit clean format, suitable for mail headers
 * @param string $text the possibly unclean text
 * @return string the text with 8-bit characters encoded in hex format
 */
function encode7bit($text)
{
	return mb_encode_mimeheader($text, "iso-8859-1", "Q");
}

/**
 * Generates a version 4 GUID
 * @return string
 */
function GUID()
{
	if (function_exists('com_create_guid') === true)
	{
		return trim(com_create_guid(), '{}');
	}

	return '{'.sprintf('%04X%04X-%04X-%04X-%04X-%04X%04X%04X', mt_rand(0, 65535), mt_rand(0, 65535), mt_rand(0, 65535), mt_rand(16384, 20479), mt_rand(32768, 49151), mt_rand(0, 65535), mt_rand(0, 65535), mt_rand(0, 65535)).'}';
}

/**
 * Generates a version 4 GUID with no punctutation
 * @return string
 */
function plainGUID()
{
	return str_replace(array("{", "}", "-"), array("", "", ""), GUID());
}

/**
 * Returns a formatted backtrace suitable for debugging output.
 */
function getBacktrace($ignore = 2)
{
	$trace = '';
	foreach (debug_backtrace() as $k => $v) {
		if ($k < $ignore) {
			continue;
		}

		array_walk($v['args'], function (&$item, $key) {
			$item = var_export($item, true);
		});

			$trace .= '#' . ($k - $ignore) . ' ' . $v['file'] . '(' . $v['line'] . '): ' . (isset($v['class']) ? $v['class'] . '->' : '') . $v['function'] . '(' . implode(', ', $v['args']) . ')' . "\n";
	}

	return $trace;
}

/**
 * Performs a Luhn validity test for credit card or IMEI numbers
 * @param string $num credit card or IMEI number
 * @return boolean true if valid, false otherwise
 */
function luhnTest($num) 
{
	$len = strlen($num);
	for ($i = $len-1; $i >= 0; $i--) 
	{
		$ord = ord($num[$i]);
		if (($len - 1) & $i) 
		{
			$sum += $ord;
		} 
		else 
		{
			$sum += $ord / 5 + (2 * $ord) % 10;
		}
	}
	return $sum % 10 == 0;
}


/**
 * Determines the number of items that two comma-separated lists have in common.
 * Useful for determining matches between checklist fields or roles.
 * @param string $a the first string to test
 * @param string $b the second string to test
 * @return integer the number of elements they have in common
 */
function countMatchingElements($a, $b)
{
	$arr1 = explode(",", $a);
	$arr2 = explode(",", $b);
	return count(array_intersect($arr1, $arr2));
}

/**
 * Outputs a list of all included files. This is provided as a tool for troubleshooting.
 * @param boolean $html whether to include HTML formatting in the output (default true)
 * @param boolean $return set to true to return the list as the return value, false to print to the output buffer
 */
function printIncludes($html = true, $return = false)
{
	if ($return)
	{
		ob_start();
	}
	
	$includes = get_included_files();
	if ($html) echo "<ul>\n";
	foreach($includes as $include)
	{
		if ($html)
		{
			echo "<li>$include</li>";
		}
		else
		{
			echo $include."\n";
		}
	}
	if ($html) echo "</ul>";
	
	if ($return)
	{
		$out = ob_get_contents();
		ob_end_clean();
		return $out;
	}
}
/**
 * @}
 */
?>