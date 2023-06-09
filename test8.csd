<CsoundSynthesizer>
<CsOptions>
-n
</CsOptions>
<CsInstruments>
; Initialize the global variables.
ksmps = 64
nchnls = 2
0dbfs = 1

chn_k "phsRate", 1
chn_k "pitch", 1

;instrument will be triggered by keyboard widget
instr 1
    aOutL = oscili:a(scale(oscili:k(1, 0.25, -1, 0), 0.2, 1), 400 * p4 + oscili:k(2, 3))
    aOutR = oscili:a(scale(oscili:k(1, 0.25, -1, 0.5), 0.2, 1), 200 * p4 + oscili:k(4, 1.5))
    outs aOutL/8, aOutR/8
endin

instr 2
    aOutL = oscili:a(scale(oscili:k(1, chnget:k("phsRate"), -1, 0), 0.2, 1), chnget:k("pitch") * p4 + oscili:k(2, 3))
    aOutR = oscili:a(scale(oscili:k(1, chnget:k("phsRate"), -1, 0.5), 0.2, 1), (chnget:k("pitch")/2) * p4 + oscili:k(4, 1.5))
    outs aOutL/8, aOutR/8
endin

</CsInstruments>
<CsScore>
;causes Csound to run for about 7000 years...
i2 0 z 0.25
i2 0 z 0.5
i2 0 z 0.75
i2 0 z 1
i2 0 z 1.25
i2 0 z 1.5
i2 0 z 1.75
i2 0 z 2
</CsScore>
</CsoundSynthesizer>
