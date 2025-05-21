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

/* 
 * Description: table list view of contact us topics
 * 
 * author: Janice Gallant for Sonjara, Inc.
 * 
 * date: 11/18/10

***************************************************************/


Fakoli::using("email");
Fakoli::usingFeature("data_view");

$menu_item = "Contact Topics";

$title = "Contact Topics";

$topics = query('ContactTopic', "ORDER BY sort_order");

$table = new DataListView($topics, "contactTopics");
$table->column("Code Name", "<a href=\"/admin/contact_topic_form?contact_topic_id={contact_topic_id}\">{topic}</a>")
		->column("Topic", "{topic}")
	  	->column("Recipients", "{recipients}");
	  	
$sites = query('Site');
if(count($sites) > 1)
	$table->column("Sites", formatSiteList);
	  	
$table->emptyMessage = "There are no contact topics defined.";
$table->sortable = false;
$table->cssStyle = "width: 100%";
$table->enableDragReorder("/action/email/reorder_contact_topics");
$table->dragText = "<span style='font-size: 10px'>Click and drag to reorder contact topics</span>";


if ($method == "POST")
{
	reorderContactTopics();
	// refresh
	redirect("/admin/contact_topics");
}

$script .= $table->writeScript();

?>
<p>Contact Us Topics are displayed to users in a drop down menu when completing the Contact Us form.</p>
<form style='display: inline' name="contact_topics_form" method="POST" action="">

<?php
$table->drawView();
?>
<br><br><input class='button' type="submit" name="reorder" value="&nbsp;Update Topic Order&nbsp;"/>&nbsp;&nbsp;
<a class='button' href='/admin/contact_topic_form'>Add a Contact Us Topic</a>
</form>
<?php 
function formatSortOrderInputField($contactTopic)
{
	return "<input type='text' name='contact_topic_{$contactTopic->contact_topic_id}' 
		value='{$contactTopic->sort_order}' size='4'/>"; 
	
}

function formatSiteList($contactTopic)
{
	$sites = $contactTopic->Sites();
	return formatItems($sites, "{site_name}", "<br>");
}

function reorderContactTopics()
{
	global $_POST;
	// reorder
	foreach($_POST as $name => $value)
	{
		if (!strncmp($name, "contact_topic_", 14))
		{
			$id = substr($name, 14);
			$contactTopic = new ContactTopic();
			$contactTopic->filter = new InclusionFilter("contact_topic_id", "sort_order");	
			$contactTopic->contact_topic_id = $id;
			$contactTopic->sort_order = $value;
			$contactTopic->save();
		}
	}
	// renumber
	$contactTopics = query('ContactTopic', "ORDER BY sort_order");
	$value = 1;
	
	foreach($contactTopics as $contactTopic)
	{
		$contactTopic->filter = new InclusionFilter("sort_order");
		$contactTopic->sort_order = $value;
		$contactTopic->save();			
		$value++;
	} // end reOrder		
}
?>
