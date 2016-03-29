-- START Version 1.0
-- END Version 1.0

-- START Version 1.1

CREATE TABLE snippet
(
	snippet_id		int(10) not null auto_increment,
	snippet_name	varchar(200) not null,
	snippet			text,
	user_id			int(10) not null default 0,
	shared			tinyint(3) not null default 0,
	last_modified	datetime,
	PRIMARY KEY(snippet_id)
) ENGINE=InnoDB;

-- END Version 1.1

-- START Version 1.2

ALTER TABLE snippet ADD COLUMN description VARCHAR(1000) AFTER snippet_name;

-- END Version 1.2