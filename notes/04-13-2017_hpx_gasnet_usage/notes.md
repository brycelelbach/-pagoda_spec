Attending: Bryce, John

Is rendevouz the thing for us to use?
    It's higher latency, but you hide latency

Scalable AM inboxes using AMOs
    We may not need this because we have a sane many-core
    Also, if you use rendevouz, you don't need big AMs, you can live with tiny unscalable buffers
Endpoints
try-fail APIs

Do we call GASNet progress from the scheduler loop or the network thread pool (that then enqueues the work)?
    From the network thread pool

Can inject directly from worker threads?


