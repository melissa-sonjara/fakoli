<?php
/**\addtogroup connectable
 * @{
 */


Fakoli::using("connectable");

class ConnectableNavigator
{
	var $id;
	var $title;
	var $source;
	var $panel;
	var $handler;
	var $enabled = true;
	
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
		$this->panel = new Panel($this->id."_panel", $url);
	}
	
	function writeScript()
	{
		$script = <<<ENDSCRIPT
<script type="text/javascript" src="/components/connectable/js/connectable_navigator.js"></script>
<script type="text/javascript">
var {$this->id}_ConnectableNavigator;
window.addEvent('load', function()
{
	{$this->id}_ConnectableNavigator = new ConnectableNavigator('{$this->id}_panel');
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
		
		if (!$this->enabled)
		{
			$icon = $obj->getConnectableDisabledIcon();
?>
			<li><img src="<?echo $icon?>" alt="<?echo $name?>" title="<?echo $name?>" class="disabled"/>
<?
			return;
		}
		
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
				echo $item->getConnectableLink($this->source)."<br/>";
			}
		}
		else
		{
			echo "There are no related $name<br/>";
		}
		
		$from = get_class($this->source).":".$this->source->get($this->source->getPrimaryKey());
		$createURL = ConnectableManager::$contextRouter . "?from=$from&to=$target";
		$createHandler = "";
		
		if (method_exists($obj, 'getConnectableCreateHandler'))
		{
			$createHandler = $obj->getConnectableCreateHandler();
			$createHandler = " onclick='{$createHandler}(\"{$createURL}\"); return false;'";
			$createURL = "#";
		}
?>
	<br/>
	<a class='button small' href='#' onclick='ConnectableNavigator.selectConnectables("<?echo $from?>", "<?echo $target?>", "<?echo $name?>"); return false'>Select...</a>&nbsp;&nbsp;&nbsp;&nbsp;<a class='button small' href='<?echo $createURL?>'<?echo $createHandler?>>Create...</a>
  </div>
  </li>
<?
	}
	
	function drawNavigator()
	{
		$targets = ConnectableManager::getTargetClasses($this->source);

		if (!$this->source->get($this->source->getPrimaryKey())) $this->enabled = false;
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
/**@}*/?>
