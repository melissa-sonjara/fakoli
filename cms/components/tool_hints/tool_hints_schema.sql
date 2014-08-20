-- Component Schema for tool_hints
--
-- Author: Andrew Green
--
-- Each version update must begin with the following:
-- START Version xx
-- and end with
-- END Version xx


-- START Version 1.0

create table tool_hint
(
	hint_id	int(10) unsigned not null auto_increment,
	code varchar(200) not null default '',
	title varchar(1000) not null default '',
	hint text null,
	primary key(hint_id)
) Engine=InnoDB;


create table tool_hint_user_status
(
	status_id int(10) unsigned  not null auto_increment,
	hint_id	int(10) unsigned  not null,
	user_id int(10) unsigned  not null,
	hidden tinyint(3) not null default 0,
	primary key(status_id)
) Engine=InnoDB;

-- END Version 1.0

-- START Version 1.1

ALTER TABLE tool_hint ADD COLUMN can_be_hidden TINYINT(3) NOT NULL DEFAULT 1;

-- END Version 1.1

-- START Version 1.2

ALTER TABLE tool_hint ADD COLUMN read_access VARCHAR(200) NOT NULL DEFAULT '';

-- END Version 1.2