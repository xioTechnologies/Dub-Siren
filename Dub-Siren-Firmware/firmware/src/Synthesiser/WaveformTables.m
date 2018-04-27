clc;
close all;
clear all;

minimumFrequency = 500; % specified minimum bandwidth-limit waveform frequency
maximumFrequency = 20000; % specified maximum bandwidth-limit waveform frequency

maximumHarmonic = ceil(maximumFrequency / minimumFrequency);
numberOfSamples = 10 * maximumHarmonic; % ensure at least 20 samples per maximum harmonic frequency

numberOfFloats = 0; % running sum for memory calculation

%% Open header file

fileWrite = fopen('WaveformTables.h', 'w');
fprintf(fileWrite, '// This file was generated using WaveformTables.m\n\n');
fprintf(fileWrite, '#define WAVEFORM_TABLE_LENGTH %i\n\n', numberOfSamples);
fprintf(fileWrite, '#define MINIMUM_FREQUENCY %ff\n\n', minimumFrequency);
fprintf(fileWrite, '#define MAXIMUM_FREQUENCY %ff\n\n', maximumFrequency);

%% Sine wave

sineTable = sin(linspace(0, 2 * pi, numberOfSamples));

fprintf(fileWrite, 'const static float sineTable[] = {');
for index = 1:numberOfSamples
    fprintf(fileWrite, '%0.6ff, ', sineTable(index));
    numberOfFloats = numberOfFloats + 1;
end
fprintf(fileWrite, '};\n\n');

figure;
hold on;
plot(sineTable, 'r');

%% Triangle wave

triangleharmonics = 1:2:(maximumFrequency / minimumFrequency);

fprintf(fileWrite, '#define NUMBER_OF_TRIANGLE_WAVEFORMS %i\n\n', length(triangleharmonics));

fprintf(fileWrite, 'const static float triangleTable[%i][%i] = {', length(triangleharmonics), numberOfSamples);

for harmonic = triangleharmonics

    coefficient = 4 * ((1 - (-1)^harmonic) / (pi^2 * harmonic^2));
    harmonicWave = coefficient * cos(linspace(0, 2 * pi * harmonic, numberOfSamples));

    if(harmonic == 1)
        triangleWave = harmonicWave;
    else
        triangleWave = triangleWave + harmonicWave;
    end

    fprintf(fileWrite, '{');
    for i = 1:numberOfSamples
        fprintf(fileWrite, '%0.6ff, ', triangleWave(i));
        numberOfFloats = numberOfFloats + 1;
    end
    fprintf(fileWrite, '},\r\n');

    plot(triangleWave, 'g');
end

fprintf(fileWrite, '};\n\n');

%% Sawtooth wave

sawtoothHarmonics = 1:(maximumFrequency / minimumFrequency);

fprintf(fileWrite, '#define NUMBER_OF_SAWTOOTH_WAVEFORMS %i\n\n', length(sawtoothHarmonics));

fprintf(fileWrite, 'const static float sawtoothTable[%i][%i] = {', length(sawtoothHarmonics), numberOfSamples);

for harmonic = sawtoothHarmonics

    coefficient = (-2 / pi) * ((-1^harmonic) / harmonic);
    harmonicWave = coefficient * sin(linspace(0, 2 * pi * harmonic, numberOfSamples));

    if(harmonic == 1)
        sawtoothWave = harmonicWave;
    else
        sawtoothWave = sawtoothWave + harmonicWave;
    end

    fprintf(fileWrite, '{');
    for i = 1:numberOfSamples
        fprintf(fileWrite, '%0.6ff, ', sawtoothWave(i));
        numberOfFloats = numberOfFloats + 1;
    end
    fprintf(fileWrite, '},\r\n');

    plot(sawtoothWave, 'b');
end

fprintf(fileWrite, '};\n\n');

%% Square wave

squareharmonics = 1:2:(maximumFrequency / minimumFrequency);

fprintf(fileWrite, '#define NUMBER_OF_SQUARE_WAVEFORMS %i\n\n', length(squareharmonics));

fprintf(fileWrite, 'const static float squareTable[%i][%i] = {', length(squareharmonics), numberOfSamples);

for harmonic = squareharmonics

    coefficient = (4 / pi) * (1 / harmonic);
    harmonicWave = coefficient * sin(linspace(0, 2 * pi * harmonic, numberOfSamples));

    if(harmonic == 1)
        squareWave = harmonicWave;
    else
        squareWave = squareWave + harmonicWave;
    end

    fprintf(fileWrite, '{');
    for i = 1:numberOfSamples
        fprintf(fileWrite, '%0.6ff, ', squareWave(i));
        numberOfFloats = numberOfFloats + 1;
    end
    fprintf(fileWrite, '},\r\n');

    plot(squareWave, 'k');
end

fprintf(fileWrite, '};\n\n');

%% Pulse wave

dutyCycle = 0.2;

fprintf(fileWrite, '#define HALF_PULSE_DUTY_CYCLE %0.6ff\n\n', dutyCycle / 2);

pulseharmonics = 1:(maximumFrequency / minimumFrequency);

fprintf(fileWrite, '#define NUMBER_OF_PULSE_WAVEFORMS %i\n\n', length(pulseharmonics));

fprintf(fileWrite, 'const static float pulseTable[%i][%i] = {', length(pulseharmonics), numberOfSamples);

for harmonic = pulseharmonics

    coefficient = 2 * (1 / (harmonic * pi)) * sin(dutyCycle * harmonic * pi);
    harmonicWave = coefficient * cos(linspace(0, 2 * pi * harmonic, numberOfSamples));

    if(harmonic == 1)
        pulseWave = harmonicWave;
    else
        pulseWave = pulseWave + harmonicWave;
    end

    adjustedPulseWave = 2 * (pulseWave - (1 - dutyCycle) + 0.5); % adjust for offset and scaling

    fprintf(fileWrite, '{');
    for i = 1:numberOfSamples
        fprintf(fileWrite, '%0.6ff, ', adjustedPulseWave(i));
        numberOfFloats = numberOfFloats + 1;
    end
    fprintf(fileWrite, '},\r\n');

    plot(adjustedPulseWave, 'm');
end

fprintf(fileWrite, '};\n\n');

%% Memory required

disp(sprintf('Memory Required = %0.1ff kB', (numberOfFloats * 4) / 1000));

%% Close header file

fclose(fileWrite);
