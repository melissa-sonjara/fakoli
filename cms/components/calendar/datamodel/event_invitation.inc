<?php
/**\addtogroup calendar
 * @{
 */


Fakoli::using("settings");

class EventInvitation extends DataItem
{
	var $fields = array(
		"event_invitation_id"	=>	Number,
		"event_id"				=>	Number,
		"sender_email"			=>	String,
		"sender_name"			=>	String,
		"subject"				=>	String,
		"message"				=>	HTML,
		"sequence_id"			=>	Number,
	);
	
	var $relations = array(
			"Event"				=>	Event,
			"Recipients"		=>	EventRecipient,
			);
	
	
	function Event()
	{
		return $this->getRelated(Event);
	}
	
	function Recipients($constraint = "")
	{
		return $this->getRelatedList(EventRecipient, "", $constraint);
	}
	
	static function create(&$event)
	{
		global $user;
		
		$invitation = new EventInvitation();
		$invitation->event_id = $event->event_id;
		$invitation->sender_email = $user->email;
		$invitation->sender_name = $user->format("{first_name} {last_name}");
		$invitation->subject = Settings::getValue("settings", "sitename") . " Event Invitation: {$event->title}";
		$invitation->save();
		
		$event->event_invitation_id = $invitation->event_invitation_id;
		$event->filter = new InclusionFilter("event_invitation_id");
		$event->save();
		
		return $invitation;
	}
	
	function __construct()
	{
		$this->table = "event_invitation";
		$this->primary_key = "event_invitation_id";
		$this->DataItem(func_get_args());
	}
}
/**@}*/?>