<CsoundSynthesizer>
<CsOptions>

</CsOptions>
<CsInstruments>
; Initialize the global variables.
ksmps = 64
nchnls = 2
0dbfs = 1

;instrument will be triggered by keyboard widget
instr 1
    aOutL = oscili:a(scale(oscili:k(1, 0.25, -1, 0), 0.2, 1), 400 + oscili:k(2, 3))
    aOutR = oscili:a(scale(oscili:k(1, 0.25, -1, 0.5), 0.2, 1), 200 + oscili:k(4, 1.5))
    outs aOutL, aOutR
endin

</CsInstruments>
<CsScore>
;causes Csound to run for about 7000 years...
i1 0 z 0.25
i1 0 z 0.5
i1 0 z 0.75
i1 0 z 1
i1 0 z 1.25
i1 0 z 1.5
i1 0 z 1.75
i1 0 z 2
</CsScore>
</CsoundSynthesizer>
