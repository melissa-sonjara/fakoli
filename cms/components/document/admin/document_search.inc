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

require_once "../../include/config.inc";
require_once "../datamodel/document.inc";
require_once "../../include/permissions.inc";
require_once "../datamodel/journal.inc";
require_once "../datamodel/region.inc";
require_once "../datamodel/topic.inc";
require_once "../datamodel/setting.inc";
require_once "../datamodel/sample.inc";
require_once "../datamodel/grp.inc";
require_once "../../framework/tree.inc";

$regions = query(Region, "ORDER BY region_id");
$topics = query(Topic, "ORDER BY topic");
$groups = query(Grp, "ORDER BY title");
$grpTopics=	groupedQuery(GrpTopicXref, "WHERE 1=1", "grp_id");
 
$gpTree = new TreeControl("grp_id");
$gpTree->width = 500;
$gpTree->height = 280;
$gpTree->selectMode = "single";
foreach($groups as $group) 
{	  
       $groupNode = new TreeNode($group->grp_id, $group->title,  $group->grp_id, false, 'bare_node_closed', 'bare_node_open', "");
	   $groupNode->leafStyle = "bare_node_leaf";
       $children = $grpTopics[$group->grp_id];
    	
		if($children) 
		{
			foreach($children as $child)
			{
				
				$topicNode =  new TreeNode('topic_id', $topics[$child->topic_id]->topic, $child->topic_id, false, "bare_node_closed", "bare_node_open", "");
		   		$topicNode->leafStyle = "bare_node_leaf";
	 			$groupNode->add($topicNode);
			}
		}
    	$gpTree->add($groupNode);

}
$script = $gpTree->writeScript();
$script .= "<link type='text/css' rel='stylesheet' href='/css/tree.css'/>";

		
$settings = query(Setting, "ORDER BY setting_id");
$samples = query(Sample, "ORDER BY sample_id");
$journals = query(Journal, "ORDER BY title");

$menu_item = "Publications";
$title = "Search Publications";






?>

<p>Select as many categories as needed to narrow your search of the Publications database.</p>
<form method="GET" action="results.php">

<table width="100%">
<tr>
<td><label>Regions:</label></td>
<td>
<select name='region_id'>
<option value="">Select region</option><?
foreach($regions as $region)
{
?>
 <option value="<?echo $region->region_id ?>"><?echo $region->region ?></option>
 
<?
}

?>
</select>
</td>
</tr>
<?php 
/*$children = $eventSubSteps[$eventStep->event_step_key];
	if($children){
		foreach($children as $child)
		{
			$childNode =  new TreeNode("event_sub_step_{$child->event_sub_step_key}", $child->name, null, false, "tree_node_closed", "tree_node_open", "event_sub_step_form.php?event_sub_step_key={$child->event_sub_step_key}&initiative_key={$initiative_key}");
			$eventStepNode->add($childNode);
		}
	}
	$eventStepTree->add($eventStepNode); */
	?>
<tr><td><label>Groups/Topics:</label></td>
<td>
<?
$gpTree->writeHTML();
?>
</td>
</tr>

<tr><td><label>Study Settings:</label></td>
<td>

<select name='setting_id'>
<option value="">Select study setting type</option>
<?
foreach($settings as $setting)
{
?>
 <option value="<?echo $setting->setting_id ?>"><?echo $setting->setting ?></option>

<?
}

?>
</select>

</td>
</tr>

<tr><td><label>Study Samples:</label></td>
<td>

<select name='sample_id'>
<option value="">Select study sample type</option>
<?
foreach($samples as $sample)
{
?>
 <option value="<?echo $sample->sample_id ?>"><?echo $sample->sample ?></option>
 
<?
}
?>
</select>

</td>
</tr>


<tr><td><label>Publication Year:</label></td>
<td>

<input type='text' name='publication_year' size='4' maxlength='4' onkeydown='return maskInput(event);'/>

</td>
</tr>

<tr><td><label>Journal:</label></td>
<td>
<select name='journal_id'>
<option value="">Select journal</option>
<? foreach($journals as $journal)
{
?>
 <option value="<?echo $journal->journal_id ?>"><?echo ellipsis($journal->title, 80, true) ?></option> 

<?
}
?>
</select>

</td>
</tr>

<tr><td><label>Text Search: </label></td>
<td>
<input type='text' name='searchText' size='60'/>
</td>
</tr>
<tr>
 <td><label>Author:</label></td>
 <td><input type="text" name="author" size="40"/></td>
</tr>
<tr><td colspan=2 align="right">

<input type="submit" class="button" value="Search">  <input class="button" type="reset" value="Clear"></td>

</tr>

</table>
</form>

<br/>
<br/>
<a href="publication_form.php" class="button"> Add a New Publication </a>


<br/>
<?

?>