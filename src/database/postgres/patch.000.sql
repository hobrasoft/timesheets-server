begin;

create table version (
    version     int
);


insert into version values (0);
create rule version_no_insert as on insert to version
    do instead update version set version = new.version;


create table users (
   "user"       int primary key,
    login       text,
    name        text,
    password    text,
    lang        text,
    enabled     boolean
);


create table categories (
    category        int primary key,
    parent_category int references categories(category) on update cascade on delete cascade,
    description     text,
    price           numeric
);


create table users_categories (
    category        int  references categories(category) on update cascade on delete cascade,
   "user"           int references users("user") on update set null on delete set null
);


create table statuses (
    status          text primary key,
    description     text not null,
    abbreviation    text,
    color           text,
    closed          boolean
);


create table status_order (
    category        int  references categories(category) on update cascade on delete cascade,
    previous_status text references statuses(status) on update cascade on delete cascade,
    next_status     text references statuses(status) on update cascade on delete cascade,
    primary key (category, previous_status, next_status)
);


create table tickets (
    ticket          int primary key,
    category        int  references categories(category) on update cascade on delete cascade,
   "user"           int references users("user") on update set null on delete set null,
    date            timestamp with time zone not null default now(),
    price           numeric,
    description     text
);


create table ticket_timesheets (
    id              int primary key,
    ticket          int references tickets(ticket) on update cascade on delete cascade,
    date_from       timestamp with time zone not null,
    date_to         timestamp with time zone not null
);


create table ticket_status (
    id              int primary key,
    ticket          int references tickets(ticket) on update cascade on delete cascade,
   "user"           int references users("user") on update set null on delete set null,
    date            timestamp with time zone not null default now(),
    description     text,
    status          text references statuses(status) on update no action on delete no action
);


create table ticket_values (
    id              int primary key,
    ticket          int references tickets(ticket) on update cascade on delete cascade,
    name            text,
    value           text
);


create table ticket_files (
    id              int primary key,
    ticket          int references tickets(ticket) on update cascade on delete cascade,
    name            text,
    type            text,
    content         text
);

commit;
