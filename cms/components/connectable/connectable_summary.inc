<?php
Fakoli::using("connectable");

class ConnectableSummary
{
	var $source;
	var $class;
	
	function __construct($source, $class = "connectable_summary")
	{
		$this->id = $id;
		$this->title = $title;
		$this->source = $source;
	}
	
	function drawTarget($target)
	{
		trace("Drawing target: $target for source ".get_class($this->source), 3);
		
		$obj = new $target;
		$name = $obj->prettifyClassName(true);
		$icon = $obj->getConnectableIcon();
		
		$count = ConnectableManager::getConnectedItemCount($this->source, $target, $obj->getConnectableConstraint());
?>
  <li><img src="<?echo $icon?>" alt="<?echo $name?>" title="<?echo $name?>" data-count="<?echo $count?>"/></li>
<?
	}
	
	function draw()
	{
		$targets = ConnectableManager::getTargetClasses($this->source);
		
?>
<ul class="<?echo $this->class?>">
<?
		foreach($targets as $target)
		{
			$this->drawTarget($target);
		}
?>
</ul>
<?
	}
}