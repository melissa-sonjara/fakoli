-- Component Schema for connectable
--
-- Author: Andrew Green
--
-- Each version update must begin with the following:
-- START Version xx
-- and end with
-- END Version xx


-- START Version 1.0

CREATE TABLE connection_record
(
	connection_id	int(11) not null auto_increment,
	source_class	varchar(200) not null,
	source_id		int(11) not null,
	target_class	varchar(200) not null,
	target_id		int(11) not null,
	PRIMARY KEY(connection_id)
) ENGINE=InnoDB;

CREATE INDEX connectable_source_idx ON connection_record (source_class, source_id);
CREATE INDEX connectable_target_idx ON connection_record (target_class, target_id);

-- END Version 1.0