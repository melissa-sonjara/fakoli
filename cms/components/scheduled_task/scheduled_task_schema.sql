-- Component Schema for scheduled_task
--
-- Author: Andy Green
--
-- Each version update must begin with the following:
-- START Version xx
-- and end with
-- END Version xx


-- START Version 1.0

CREATE TABLE scheduled_task
(
	task_id		int(10) not null auto_increment,
	component	varchar(200) not null,
	task_name	varchar(200) not null,
	schedule 	varchar(1000) not null default '',
	active		tinyint(3) not null default 0,
	next_run	datetime,
	PRIMARY KEY(task_id)
) ENGINE=InnoDB;

CREATE TABLE scheduled_task_log_entry
(
	log_entry_id	int(10) not null auto_increment,
	task_id			int(10) not null,
	log_date		datetime,
	log				longtext,
	status			varchar(1000) not null default '',
	PRIMARY KEY(log_entry_id)
) ENGINE=InnoDB;

ALTER TABLE scheduled_task_log_entry ADD INDEX log_entry_idx (task_id ASC, log_date DESC);

-- END Version 1.0

-- START Version 1.1

ALTER TABLE scheduled_task DROP COLUMN next_run;

-- END Version 1.1