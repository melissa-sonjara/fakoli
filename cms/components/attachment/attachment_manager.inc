<?php
/**\addtogroup attachment
 * @{
 */


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


class AttachmentManager
{
	function __construct()
	{
		
	}
	
	static function deleteAttachment($attachment, $xrefClass = "")
	{
		if(!$attachment)
			return;
			
		global $config;
		
		$dir = $config["attachmentdir"];
		$base = $config['uploadbase'];
			
		$file = "$base/$dir{$attachment->local_filename}";

		if (file_exists($file))
		{
			unlink($file);
		}
		
		if($xrefClass)
			AttachmentManager::deleteAttachmentXref($attachment->attachment_id, $xrefClass);
			
		$attachment->delete();		
	}
	
	static function getAttachmentLink($attachment)
	{
 		$icon = getDocIcon($attachment->filename);
 		
 		$html .= "<img src=\"$icon\" alt=\"Icon\" style=\"display:inline-block;vertical-align:middle\"/>&nbsp;
		<a href=\"/action/attachment/download?attachment_id={$attachment->attachment_id}\">{$attachment->filename}</a>&nbsp;
		({$attachment->file_size})";
	
 		return $html;
	}
	
	
	static function deleteAttachmentXref($attachment_id, $xrefClass)
	{
		
		$xrefs = Query::create($xrefClass, "WHERE attachment_id=:attachment_id")
			->bind(":attachment_id", $attachment_id)
				->execute(); 
				
		if(count($xrefs) > 0)
		{
			foreach($xrefs as $xref)
				$xref->delete();
		}
	}
	
	static function upgradeComponent($version)
	{
		$mgr = new AttachmentUpgradeManager();
		$mgr->upgrade($version);
	}	
}
/**@}*/?>
