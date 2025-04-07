(define (domain graph-with-agents)
        (:requirements :strips :typing
                ;:durative-actions
        )
        (:types vertex agent)
        (:predicates
              (occupied ?v - vertex)
              (is-at ?a - agent ?v - vertex)
              (edge ?a ?b - vertex)
        )

        (:action move
                :parameters (?a - agent ?s - vertex ?d - vertex)
                :precondition (and
                        (is-at ?a ?s)
                        (occupied ?s)
                        (not (occupied ?d))
                        (or
                                (edge ?d ?s)
                                (edge ?s ?d)
                        )
                )
                :effect (and
                        (not (is-at ?a ?s))
                        (is-at ?a ?d)
                        (not (occupied ?s))
                        (occupied ?d)
                )
        )
;
;       (:durative-action move
;           :parameters (?a - agent ?s - vertex ?d - vertex)
;           :duration (= ?duration 1)
;           :condition (and
;               (at start (is-at ?a ?s))
;               (at start (occupied ?s))
;               (at end (not (occupied ?d)))
;               (at start (or
;                   (edge ?d ?s)
;                   (edge ?s ?d)
;               ))
;           )
;           :effect (and
;               (at start (not (is-at ?a ?s)))
;               (at end (is-at ?a ?d))
;               (at start (not (occupied ?s)))
;               (at end (occupied ?d))
;           )
;       )
)

