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

Fakoli::using("fileshare", "document");
Fakoli::usingFile("framework/directory_tree.inc");

require_once realpath(dirname(__FILE__)."/../document/document_manager.inc");

class FileshareManager extends DocumentManager
{
	var $library;
	var $docs;
	
	function FileshareManager($library, $keyword = "")
	{	
		$this->library = $library;
		if($keyword)
		{
			$keyword = strtolower($keyword);
			$constraint = "WHERE LOWER(keywords) like '%$keyword%'";
		} 
		if ($this->library->document_library_id) 
		{
			$this->documents = $library->Documents("$constraint ORDER BY title");
		}
		else
		{
			$this->documents = array();
		}
	}
	
	function folderPermission($folder)
	{
		$permission = false;
		
		if($this->library->allowAccess())
			$permission = true;
			
		trace("folderPermission: $folder ($permission)", 3);
		
		return $permission;
	}
	
	/*
	 * Override DocumentManager's permissions.
	 */
	function drawButtons()
	{
?>		<div id="buttons">
		<button id="download" class="button" disabled="disabled"  onclick="downloadFile()"> Download File </button>&nbsp;&nbsp;
		
<?php
if(!checkRole("visitor"))
{ 
?>
		<button id="edit" class="button" disabled="disabled" onclick="editFile()">Edit Details...</button>&nbsp;&nbsp;
		<button id="upload" class="button" onclick="uploadFile()">Upload a File...</button>
<?
}		
		if ($this->library->isOwner() || checkRole("fileshare,admin"))
		{
?>
		<button id="deleteButton" class="button" disabled="disabled" onclick="deleteFile()">Delete File</button>&nbsp;&nbsp;
		<br><br>
		<button id="createFolder" class="button" onclick="createFolder()">Create Folder...</button>&nbsp;&nbsp;
		<button id="deleteFolder" class="button" onclick="deleteFolder()">Delete Folder</button>
<?php			
		}
?></div><?php
	}
	
	
	function writeScript()
	{
		ob_start();
?>
<script type="text/javascript">

function uploadFileshareFile()
{
	docManagerDlg.show(function() {}, "/action/document/upload?document_library_id=<?echo $this->library->document_library_id?>");
}

</script>
<?
		$script .= ob_get_contents();
		ob_end_clean();
		$script .= parent::writeScript();
		
		return $script;		
	}	

	static function libraryTabs($key = "")
	{
		$tabs = array(
			"Details"			=>	"fileshare_library_form",
			"Add Members"		=>	"fileshare_library_members",
			"Manage Files"		=>	"fileshare_library_files",
			"Find Files"		=>	"fileshare_library_search"
			);

		$qs = ($key) ? "document_library_id=$key" : ""; 
		$tabs = new TabBar("tabs", $tabs, $qs);
		$tabs->cssClass = "fileshare";
		return $tabs;
	}
	
	static function getStyles()
	{
		$styles .= "<link href=\"/fakoli/css/tabs.css\" rel=\"stylesheet\"/>\n";
		$styles .= "<link href=\"/fakoli/css/fileshare.css\" rel=\"stylesheet\"/>\n";
		
		return $styles;
	}
	
	static function drawMemberSearchForm($library)
	{
?>
<div id="role" style="float: right; width: 30%; z-index: 2">
<a class="button" href="#" onclick="new LibraryManager().openPermissionsDialog(<?php echo $library->document_library_id ?>); return false;">Set Global Access</a>
</div>
<div id="member_search_form">
<label for='name'>Add Members</label>
<input id='LibraryGroupMembers_form_name' type='text' name='name' autocomplete='off'  value='' size='20'/>
<br>
<div style="float: left; padding-top: 8px; padding-bottom: 5px; margin-bottom: 10px"><span class="ann">Type a user's name or email address to add it to the list.</span></div>
</div>
<?php		
	}	
	
	/*
	 * @members - users with fileshare privileges
	 * 
	 */
	static function constructListMembers($members, $document_library_id)
	{
		if(count($members) > 0)
		{
			foreach($members as $member)
			{
				echo FileshareManager::constructListMember($member, $document_library_id);			
			}
		}		
	}
	

	static function constructListMember($member, $document_library_id)
	{
		$text = "<div id=\"user_id_{$member->user_id}\" class=\"list_member\">&nbsp;{$member->getFullName()}&nbsp;";
		$text .= "<a href=\"#\" class=\"list_action\""; 
		$text .= "onclick=\"new LibraryManager().removeMember($document_library_id, {$member->user_id}); return false;\">&nbsp;\n";
		$text .= "<span class=\"list_action\">x</span></a>\n";	
		$text .= "</div>\n";
		return $text;
	}

	function formatDocumentTitle($document)
	{
		$document_id = $document->document_id;
		
		$link .= "<a href=\"/action/document/download?document_id={$document->title}\">{$file}
		</a>&nbsp;&nbsp;\n";
		
		// Doc edit icon after the title
		$link .= "<a href=\"#\" onclick=\"new LibraryManager().editDocumentDetailsDialog($document_id); return false;\"><img src=\"/templates/images/icon_edit.gif\" style=\"border: none;display: inline-block;vertical-align: middle;padding-right: 4px\"></a>\n";

		// Doc delete link
		if($document->fileshareAllowDelete())
			$link .= "<a href=\"#\" onclick=\"new LibraryManager().deleteFile($document_id); return false;\"><img alt=\"delete\" src=\"/fakoli/images/delete.gif\"><a href=\"#\" onclick=\"new LibraryManager().deleteFile($document_id); return false;\"></a>\n";
					
		return $link;
	}	
	
	/*
	 * If the document file path is very long, substitute
	 * "..." in the middle, rather than the end b/c the
	 * file name is that most important.
	 */
	function formatDocumentFileLink($document)
	{
		$document_id = $document->document_id;

		$file = basename($document->file);
		$icon = getDocIcon($document->file);
		$link = "<img src=\"{$icon}\" alt=\"Icon\" style=\"display:inline-block;vertical-align:middle\"/>&nbsp;"; 
		$link .= "<a href=\"/action/document/download?document_id={$document_id}\">{$document->title}
		</a>&nbsp;&nbsp;\n";
		
		$link .= "<a href=\"#\" onclick=\"new LibraryManager().editDocumentDetailsDialog($document_id); return false;\"><img src=\"/templates/images/icon_edit.gif\" style=\"border: none;display: inline-block;vertical-align: middle;padding-right: 4px\"></a>\n";

		// delete link
		if($document->fileshareAllowDelete())
			$link .= "<a href=\"#\" onclick=\"new LibraryManager().deleteFile($document_id); return false;\"><img alt=\"delete\" src=\"/fakoli/images/delete.gif\"><a href=\"#\" onclick=\"new LibraryManager().deleteFile($document_id); return false;\"></a>\n";
		
		return $link;
	}	
	
	/*
	 * Get the libraries the user is linked to either by
	 * fileshare_user_xref, by being the owner, or by
	 * their role.
	 * 
	 * If the library allows visitors read access then
	 * show to nonusers.
	 */
	static function getSharedLibraries()
	{
		global $user;
		
		$userXref = new FileshareUserXref();
		
		
		if(!$user)
			$user = FileshareManager::setVisitor();
		$roleLibraries = Query::create(DocumentLibrary, "ORDER BY name")->execute();
		$roleLibraries = filterByRole($roleLibraries, "allow_access");
		
		$constraint = "WHERE owner_id=:u or document_library_id in (select document_library_id from {$userXref->table} where user_id=:u)";
			
		$libraries = Query::create(DocumentLibrary, "$constraint ORDER BY name")
			->bind(":u", $user->user_id)
			->execute();
			
		if(count($libraries) == 0)
			$libraries = array();
			
		if(count($roleLibraries) > 0)
		{
			foreach($roleLibraries as $rLibrary)
				$libraries[] = $rLibrary;
		}
		
		return removeDuplicates($libraries, "document_library_id");
	}	
	
	static function getLibraryLastModified($library)
	{
		$documents = Query::create(Document, "WHERE document_library_id=:l ORDER BY last_modified DESC LIMIT 1")
			->bind(":l", $library->document_library_id)
			->execute();
			
		return (count($documents) > 0) ? formatDateTime12Hr($documents[0]->last_modified) : null;
	}		
	
	static function getDocumentLastModified($document)
	{
		return formatDateTime12Hr($document->last_modified);
	}		
	
	
	static function getLibraryLastModifiedBy($library)
	{
		$documents = Query::create(Document, "WHERE document_library_id=:l ORDER BY last_modified DESC LIMIT 1")
			->bind(":l", $library->document_library_id)
			->execute();
			
		$owner = (count($documents) > 0) ? $documents[0]->Owner() : null;
		
		return ($owner) ? $owner->getFullName() : null;
	}	

	/*
	 * Create the identifier by changing library name to 
	 * all lower case, replacing blanks with "_" and any
	 * removing all other nonalphnumeric characters.
	 */
	static function setLibraryIdentifier(&$form)
	{
		$library = $form->data;
		
		$name = preg_replace("/ /", "_", strtolower($library->name));
		$library->identifier = preg_replace("/[^A-Za-z0-9_]/", "", $name);
		
		$library->filter = new InclusionFilter("identifier");
		$library->save();
	}

	static function getDocumentAuthor($document)
	{
		$owner = $document->Owner();
		return ($owner) ? $owner->getFullName() : "";
	}
	
	static function getGlobalPermissionOptions()
	{
		$options = array(fileshare_only_group_members_read	=>  "Limit Access to Group Members",
						 fileshare_all_site_members_read	=>	"Allow All Site Members to View",
						 fileshare_all_visitors_read	=>	"Allow All Site Members and Site Visitors to View"
						);		
		return $options;
	}
	
	static function setVisitor()
	{
		$userClass = UserManager::getUserClass();
		$user = new $userClass();
		$user->user_id = 0;
		$user->role = "visitor";
		
		return $user;
	}
	
	static function upgradeComponent()
	{
		$mgr = new FileshareUpgradeManager();
		$mgr->upgrade();
	}	
}
