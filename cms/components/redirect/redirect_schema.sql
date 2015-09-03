-- Component Schema for redirect
--
-- Author: Andy Green
--
-- Each version update must begin with the following:
-- START Version xx
-- and end with
-- END Version xx


-- START Version 1.0

CREATE TABLE redirect
(
	redirect_id		int(10) not null auto_increment,
	redirect_from 	varchar(1000) not null,
	redirect_to 	varchar(1000) not null,
	user_id			int(10) not null,
	active			tinyint(3) not null default 0,
	last_modified	datetime not null,
	PRIMARY KEY(redirect_id)
) ENGINE=InnoDB;

-- END Version 1.0

-- START Version 1.1

ALTER TABLE redirect ADD COLUMN override tinyint(3) not null default 0;

-- END Version 1.1

-- START Version 1.2

ALTER TABLE redirect ADD COLUMN exemption_flag varchar(50) not null default '';
ALTER TABLE redirect ADD COLUMN exempt_roles varchar(1000) not null default '';

-- END Version 1.2