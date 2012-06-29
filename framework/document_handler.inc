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

/**
 * MIME types allowable for document uploads 
 */
$doc_types = array( "application/msword", 
		    "application/vnd.ms-excel", 
		    "application/excel",
		    "application/ms-excel",
		    "application/pdf",
			"application/zip",
		    "application/vnd.ms-powerpoint",
		    "application/ms-powerpoint",
		    "application/mspowerpoint",
		    "text/html",
		    "text/rtf",
			"audio/mpeg",
			"video/flv",
		);

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
        ".png" => "PNG Image File",
		".xlsx" => "Microsoft Excel Spreadsheet",
	    ".pptx" => "Microsoft Powerpoint Presentation",
		".docx" => "Microsoft Word Document",
		".mp3"	=> "MP3 Audio File",
		".flv"	=>	"Flash Video",
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
        ".png" => "image/x-png",
		".xlsx" => "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet",
	    ".pptx" => "application/vnd.openxmlformats-officedocument.presentationml.presentation",
		".docx" => "application/vnd.openxmlformats-officedocument.wordprocessingml.document",
		".mp3" => "audio/mpeg",
		".flv"	=>	"video/x-flv",
);

$doc_icons = array(	".doc"		=>	"/fakoli/images/msword_icon.png",
					".docx"		=>	"/fakoli/images/msword_icon.png",
					".xls"		=>	"/fakoli/images/msexcel_icon.png",
					".xlsx"		=>	"/fakoli/images/msexcel_icon.png",
					".pdf"		=>	"/fakoli/images/pdf_icon.png",
					".ppt"		=>	"/fakoli/images/ppt_icon.png",
					".pptx"		=>	"/fakoli/images/ppt_icon.png",
					".mp3"		=>	"/fakoli/images/mp3.png",
					".flv"		=>	"/fakoli/images/icon_video.png",
					"default"	=>	"/fakoli/images/file_icon.png");


class DocumentHandler
{
	function DocumentHandler()
	{
		
	}
	
	/**
 	* Returns a human-readable type name for a document, 
 	* based on the file extension of the supplied file name.
 	* 
 	* @param string $doc the filename of the document
 	*/
	static function getDocType($doc)
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
	static function getDocSize($doc)
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
	

	/**
	 * Replaces getMimeType because name conflicts
	 * with getMIMEType.
	 * 
	 * @param $name
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
	
	static function getDocIcon($doc)
	{
		global $doc_icons;
		$n = strtolower($doc);
		$extension = substr($n, strrpos($n, "."));
		$icon = $doc_icons[$extension];
		if (!$icon) $icon = "/fakoli/images/file_icon.png";
		return $icon;
	}	
	
	/**
	 * Retrieves a graphic icon appropriate for the given file
 	* @param $file the name of the file
	 * @return string URL of the graphic icon for the file
	*/
	static function getIcon($file)
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
	
	static function getMIMEType($filename) 
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
}
?>