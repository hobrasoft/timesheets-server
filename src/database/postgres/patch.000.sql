begin;

create table version (
    version     int
);


insert into version values (0);
create rule version_no_insert as on insert to version
    do instead update version set version = new.version;


create table users (
   "user"       serial primary key,
    login       text,
    name        text,
    password    text,
    lang        text,
    enabled     boolean,
    admin       boolean
);


insert into users (login, name, password, lang, enabled, admin) values ('admin', 'Administrator', '21232f297a57a5a743894a0e4a801fc3', 'en', true, true);

create table categories (
    category        serial primary key,
    parent_category int references categories(category) on update cascade on delete cascade,
    description     text,
    price           numeric
);


create table users_categories (
    id              serial primary key,
    category        int  references categories(category) on update cascade on delete cascade,
   "user"           int references users("user") on update set null on delete set null,
    unique (category, "user")
);


create table statuses (
    status          text primary key,
    description     text not null,
    abbreviation    text,
    color           text,
    closed          boolean,
    can_be_run      boolean,
    ignored         boolean
);


create table status_order (
    id              serial primary key,
    category        int  references categories(category) on update cascade on delete cascade,
    previous_status text references statuses(status) on update cascade on delete cascade,
    next_status     text references statuses(status) on update cascade on delete cascade
);

create unique index status_order_null_index on status_order(previous_status, next_status)
    where category is null;

create unique index status_order_nontull_index on status_order(category, previous_status, next_status)
    where category is not null;


create table tickets (
    ticket          serial primary key,
    category        int not null references categories(category) on update cascade on delete cascade,
   "user"           int not null references users("user") on update set null on delete set null,
    date            timestamp with time zone not null default now(),
    price           numeric,
    description     text
);


create table ticket_timesheets (
    id              serial primary key,
    ticket          int not null references tickets(ticket) on update cascade on delete cascade,
   "user"           int not null references users("user") on update set null on delete set null,
    date_from       timestamp with time zone not null,
    date_to         timestamp with time zone
);


create table ticket_status (
    id              serial primary key,
    ticket          int not null references tickets(ticket) on update cascade on delete cascade,
   "user"           int not null references users("user") on update set null on delete set null,
    date            timestamp with time zone not null default now(),
    description     text,
    status          text references statuses(status) on update no action on delete no action
);


create table ticket_values (
    id              serial primary key,
    ticket          int not null references tickets(ticket) on update cascade on delete cascade,
   "user"           int not null references users("user") on update set null on delete set null,
    date            timestamp with time zone not null default now(),
    name            text,
    value           json
);


create table ticket_files (
    id              serial primary key,
    ticket          int not null references tickets(ticket) on update cascade on delete cascade,
   "user"           int not null references users("user") on update set null on delete set null,
    date            timestamp with time zone not null default now(),
    name            text,
    type            text,
    content         bytea
);


create table client_settings (
    multiple_timesheets         bool,
    show_price                  bool,
    can_change_category         bool,
    edit_categories             bool,
    show_multiple_timesheets    bool,
    show_show_price             bool,
    show_can_change_category    bool,
    show_edit_categories        bool
);

commit;
