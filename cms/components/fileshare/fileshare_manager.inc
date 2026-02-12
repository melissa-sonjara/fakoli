<?php
/**\addtogroup fileshare
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

Fakoli::using("fileshare", "document");
Fakoli::usingFile("framework/directory_tree.inc");

require_once realpath(dirname(__FILE__)."/../document/document_manager.inc");

class FileshareManager extends DocumentManager
{
	var $library;
	var $docs;
	
	function __construct($library, $keyword = "")
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
		
		if(!count($this->documents))
		{
			$this->emptyMessage = "There are no documents in {$this->library->name}";
			if($keyword)
			{
				$this->emptyMessage .= " that match the keyword search '{$keyword}'</i>";
			}
			$this->emptyMessage .= ".";
		}
	}
	
	static function libraryTabs($key = "")
	{
		$tabs = array(
			"Details"			=>	"fileshare_library_form",
			"Add Members"		=>	"fileshare_library_members",
			"Files"				=>	"fileshare_library_files"
			);

		$qs = ($key) ? "document_library_id=$key" : ""; 
		$tabs = new TabBar("tabs", $tabs, $qs);
		$tabs->cssClass = "fileshare";
		return $tabs;
	}
	
	
	/**
	 * For Members tab
	 * 
	 * @param unknown $library
	 */
	static function drawMemberSearchForm($library)
	{
?>
<div id="role" style="float: right; width: 30%; z-index: 2; padding: 2px 0">
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
	
	function createUploadedByFacet()
	{
		$facet = new MultiSelectFacetFilter("owner_id", array(Document	=>	"{Owner.first_name} {Owner.last_name}"));
	
		$facet->dropdownMaxWidth = "160px";
		$facet->width = "200px";
		$facet->height = "240px";
	
		return $facet;
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
		$file = $document->file;
		
		$type = getDocType($file);
		$icon = getDocIcon($file);
		
		$library = $document->DocumentLibrary();
		$size = $library->getFileSize($document);
		
		// All users who can view and access the details dialog. Only users with edit access
		// can edit it; otherwise read only.
		$details_icon = ($document->fileshareAllowEdit()) ? "icon_edit.png" : "icon_view.png"; 
		$details_link  = "<a href=\"edit_document?document_id={document_id}\" onclick=\"new LibraryManager().editDocumentDetailsDialog({document_id}); return false;\"><img src=\"/fakoli/images/{$details_icon}\" style=\"border: none;display: inline-block;vertical-align: middle;padding-right: 4px\"></a>\n";
			
		if($document->fileshareAllowDelete())
		{
			$delete_link .= "<a href=\"delete_document?document_id={document_id}\" onclick=\"new LibraryManager().deleteFile({document_id}); return false;\"><img alt=\"delete\" src=\"/fakoli/images/icon_delete_trash.png\"></a>\n";
		}
		
		return $document->format("<span class='document_title'>{title}</span><br/><img style='display:inline-block;vertical-align: middle' src='$icon' alt='$type'/> <a href='/action/document/download?document_id={document_id}'>Download</a>&nbsp;$size&nbsp;<span style='display: inline-block'>&nbsp;{$details_link}&nbsp;{$delete_link}</span>");
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
		$roleLibraries = Query::create('DocumentLibrary', "ORDER BY name")->execute();
		$roleLibraries = filterByRole($roleLibraries, "allow_access");
		
		$constraint = "WHERE owner_id=:u or document_library_id in (select document_library_id from {$userXref->table} where user_id=:u)";
			
		$libraries = Query::create('DocumentLibrary', "$constraint ORDER BY name")
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
		$documents = Query::create('Document', "WHERE document_library_id=:l ORDER BY last_modified DESC LIMIT 1")
			->bind(":l", $library->document_library_id)
			->execute();
			
		return (count($documents) > 0) ? $documents[0]->format("{last_modified:short}") : null;
	}		
	
	static function getLibraryLastModifiedBy($library)
	{
		$documents = Query::create('Document', "WHERE document_library_id=:l ORDER BY last_modified DESC LIMIT 1")
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
	
	static function onDeleteFileshareLibary($library)
	{
		$mgr = new DocumentManager($library);
		$mgr->deleteDocumentLibrary();
		redirect("fileshare_libraries");
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
		$mgr = new UserManager();
		$userClass = $mgr->getUserClass();
		$user = new $userClass();
		$user->user_id = 0;
		$user->role = "visitor";
		
		return $user;
	}

	/**
	 * Respond to fired event DeleteUser
	 *
	 * Delete any records in this component that have dependencies on user object.
	 *
	 * @param obj $user - class SiteUser or custom user class
	 */
	static function deleteUser($user)
	{
		$pk = $user->getPrimaryKey();
		$user_id = $user->$pk;
		
		trace("Component fileshare is deleting objects dependent on user_id {$user_id}", 3);
	
		$xref = new FileshareUserXref();
		$xref->delete("WHERE user_id={$user_id}");
		
		return $user;
	}
	
	static function upgradeComponent($version)
	{
		$mgr = new FileshareUpgradeManager();
		$mgr->upgrade($version);
	}	
}

/**@}*/?>
