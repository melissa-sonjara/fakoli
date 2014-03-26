<?php
Fakoli::using("connectable");

class ConnectableNavigator
{
	var $id;
	var $title;
	var $source;
	var $panel;
	var $handler;
	
	function __construct($id, $title, $source, $handler)
	{
		$this->id = $id;
		$this->title = $title;
		$this->source = $source;
		$this->handler = $handler;
		$this->createPanel();
	}
	
	function createPanel()
	{
		$url = $this->handler."?".($this->source->getPrimaryKey())."=".($this->source->get($this->source->getPrimaryKey()));
		
		trace($url, 3);
		$this->panel = new Panel($this->id, $url, "class='connectable_navigator'");
	}
	
	function writeScript()
	{
		$script = <<<ENDSCRIPT
<script type="text/javascript" src="/components/connectable/js/connectable_navigator.js"></script>
<script type="text/javascript">
window.addEvent('load', function()
{
	new ConnectableNavigator('{$this->id}');
});
</script>
ENDSCRIPT;
		return $script;
	}
	
	function drawTarget($target)
	{
		trace("Drawing target: $target for source ".get_class($this->source), 3);
		
		$obj = new $target;
		$name = $obj->prettifyClassName(true);
		$icon = $obj->getConnectableIcon();
		
		$items = ConnectableManager::getConnectedItems($this->source, $target, $obj->getConnectableConstraint());
		$count = count($items);
?>
  <li><img src="<?echo $icon?>" alt="<?echo $name?>" title="<?echo $name?>" data-count="<?echo $count?>"/>
  <div>
  <span class='title'><?echo $name?></span><br/>
<?
		if ($count > 0)
		{
			foreach($items as $item)
			{
				echo $item->getConnectableLink()."<br/>";
			}
		}
		else
		{
			echo "There are no related $name<br/>";
		}
		
		$from = get_class($this->source).":".$this->source->get($this->source->getPrimaryKey());
		$createURL = ConnectableManager::$contextRouter . "?from=$from&to=$target";
?>
	<br/>
	<a class='button small' href='#'>Select...</a>&nbsp;&nbsp;&nbsp;&nbsp;<a class='button small' href='<?echo $createURL?>'>Create...</a>
  </div>
  </li>
<?
	}
	
	function drawNavigator()
	{
		$targets = ConnectableManager::getTargetClasses($this->source);
		
?>
<div id="<?echo $this->id?>" class="connectable_navigator">
<span class="title"><?echo $this->title?></span>
<ul>
<?
		foreach($targets as $target)
		{
			$this->drawTarget($target);
		}
?>
</ul>
</div>
<?
	}
	
	function draw()
	{
		$this->panel->draw();
	}
}