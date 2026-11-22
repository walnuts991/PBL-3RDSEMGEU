
// ==========================================
// GRIDGUARD - INTERACTIVE FRONTEND
// Demo simulations using sample project data
// ==========================================

const navButtons = document.querySelectorAll(".nav");
const pages = document.querySelectorAll(".page");
const pageTitle = document.getElementById("page-title");

const stations = {
    1: "North",
    2: "Central",
    3: "South",
    4: "East"
};

const lines = {
    1: { from: 1, to: 2, distance: 10, capacity: 100 },
    2: { from: 2, to: 3, distance: 15, capacity: 120 },
    3: { from: 1, to: 3, distance: 20, capacity: 80 },
    4: { from: 3, to: 4, distance: 8, capacity: 90 }
};

const disabledLines = new Set();


// ==========================================
// PAGE NAVIGATION
// ==========================================

function openPage(pageId) {
    const page = document.getElementById(pageId);
    if (!page) return;

    pages.forEach(p => p.classList.remove("active"));
    navButtons.forEach(b => b.classList.remove("active"));

    page.classList.add("active");

    const activeButton = document.querySelector(
        `.nav[data-page="${pageId}"]`
    );

    if (activeButton) {
        activeButton.classList.add("active");
        pageTitle.textContent =
            activeButton.querySelector("span")?.textContent ||
            activeButton.textContent.trim();
    }

    window.scrollTo({ top: 0, behavior: "smooth" });
}

navButtons.forEach(button => {
    button.addEventListener("click", () => {
        openPage(button.dataset.page);
    });
});

document.querySelectorAll("[data-go]").forEach(button => {
    button.addEventListener("click", () => {
        openPage(button.dataset.go);
    });
});


// ==========================================
// TOAST NOTIFICATIONS
// ==========================================

function showToast(message) {
    const toast = document.getElementById("toast");

    toast.textContent = message;
    toast.classList.add("show");

    clearTimeout(window.gridToastTimer);

    window.gridToastTimer = setTimeout(() => {
        toast.classList.remove("show");
    }, 2600);
}


// ==========================================
// NETWORK STATUS
// ==========================================

function updateNetworkDisplay() {
    const activeCount = Object.keys(lines).length - disabledLines.size;

    document.getElementById("line-count").textContent =
        `${String(activeCount).padStart(2, "0")} / 04`;

    document.querySelectorAll(".grid-line").forEach(line => {
        const id = Number(line.dataset.line);
        line.classList.toggle("failed", disabledLines.has(id));
    });

    document.querySelectorAll(".line-status").forEach(status => {
        const id = Number(status.dataset.status);
        const active = !disabledLines.has(id);

        status.textContent = active ? "● Active" : "● Failed";
        status.style.color = active ? "#43d6a0" : "#ff657a";
    });

    document.querySelectorAll(".line-toggle").forEach(button => {
        const id = Number(button.dataset.line);
        const isOff = disabledLines.has(id);

        button.textContent = isOff ? "Switch ON" : "Switch OFF";
        button.classList.toggle("is-off", isOff);
    });
}


// ==========================================
// LINE ON/OFF SIMULATION
// ==========================================

document.querySelectorAll(".line-toggle").forEach(button => {
    button.addEventListener("click", () => {
        const id = Number(button.dataset.line);

        if (disabledLines.has(id)) {
            disabledLines.delete(id);
            showToast(`Transmission Line ${id} restored.`);
        } else {
            disabledLines.add(id);
            showToast(`Transmission Line ${id} switched OFF.`);
        }

        updateNetworkDisplay();
    });
});


// ==========================================
// SUBSTATION INSPECTION
// ==========================================

document.querySelectorAll(".station").forEach(station => {
    station.addEventListener("click", () => {
        const id = Number(station.dataset.station);

        document.querySelectorAll(".station").forEach(s => {
            s.classList.remove("selected");
        });

        station.classList.add("selected");

        const supply = id === 1 ? "200 MW" : "0 MW";
        const demand = id === 1 ? "0 MW" :
            ({ 2: "100 MW", 3: "80 MW", 4: "60 MW" })[id];

        document.getElementById("station-info").textContent =
            `Substation ${id}: ${stations[id]} | ` +
            `Supply: ${supply} | Demand: ${demand}`;

        showToast(`${stations[id]} substation selected.`);
    });
});


// ==========================================
// GRAPH HELPERS
// ==========================================

function getShortestPath(source, destination) {
    const distance = [Infinity, Infinity, Infinity, Infinity, Infinity];
    const previous = [null, null, null, null, null];
    const visited = new Set();

    distance[source] = 0;

    while (visited.size < 4) {
        let current = null;

        for (let i = 1; i <= 4; i++) {
            if (!visited.has(i) &&
                (current === null || distance[i] < distance[current])) {
                current = i;
            }
        }

        if (current === null || distance[current] === Infinity) break;
        if (current === destination) break;

        visited.add(current);

        for (const [key, edge] of Object.entries(lines)) {
            const id = Number(key);

            if (disabledLines.has(id)) continue;

            let neighbor = null;

            if (edge.from === current) neighbor = edge.to;
            if (edge.to === current) neighbor = edge.from;

            if (neighbor === null || visited.has(neighbor)) continue;

            const newDistance = distance[current] + edge.distance;

            if (newDistance < distance[neighbor]) {
                distance[neighbor] = newDistance;
                previous[neighbor] = current;
            }
        }
    }

    if (distance[destination] === Infinity) return null;

    const path = [];
    let current = destination;

    while (current !== null) {
        path.unshift(current);
        current = previous[current];
    }

    return { path, distance: distance[destination] };
}


// ==========================================
// SHORTEST PATH CALCULATOR
// ==========================================

document.getElementById("calculate-path").addEventListener("click", () => {
    const source = Number(document.getElementById("path-source").value);
    const destination = Number(
        document.getElementById("path-destination").value
    );

    if (source === destination) {
        showToast("Choose two different substations.");
        return;
    }

    const result = getShortestPath(source, destination);

    const routeElement = document.getElementById("path-route");
    const distanceElement = document.getElementById("path-distance");
    const messageElement = document.getElementById("path-message");

    if (!result) {
        routeElement.textContent = "No available route";
        distanceElement.innerHTML = "-- <span>km</span>";
        messageElement.textContent =
            "The selected substations are disconnected in the current demo network.";
        showToast("No route found.");
        return;
    }

    const routeNames = result.path.map(id => stations[id]);

    routeElement.textContent = routeNames.join(" → ");
    distanceElement.innerHTML =
        `${result.distance.toFixed(2)} <span>km</span>`;

    messageElement.textContent =
        `Route contains ${result.path.length} substations. Calculated using the demo network distances.`;

    showToast("Shortest route calculated!");
});


// ==========================================
// MAXIMUM FLOW - EDMONDS-KARP DEMO
// ==========================================

function calculateMaximumFlow(source, sink) {
    if (source === sink) return null;

    const capacity = Array.from(
        { length: 5 },
        () => Array(5).fill(0)
    );

    for (const [key, edge] of Object.entries(lines)) {
        const id = Number(key);
        if (disabledLines.has(id)) continue;

        capacity[edge.from][edge.to] += edge.capacity;
        capacity[edge.to][edge.from] += edge.capacity;
    }

    const residual = capacity.map(row => [...row]);
    let maxFlow = 0;
    const cutEdges = [];

    while (true) {
        const parent = Array(5).fill(-1);
        const queue = [source];
        parent[source] = source;

        while (queue.length && parent[sink] === -1) {
            const current = queue.shift();

            for (let next = 1; next <= 4; next++) {
                if (parent[next] === -1 && residual[current][next] > 0) {
                    parent[next] = current;
                    queue.push(next);
                }
            }
        }

        if (parent[sink] === -1) break;

        let pathFlow = Infinity;
        let current = sink;

        while (current !== source) {
            const prev = parent[current];
            pathFlow = Math.min(pathFlow, residual[prev][current]);
            current = prev;
        }

        current = sink;

        while (current !== source) {
            const prev = parent[current];
            residual[prev][current] -= pathFlow;
            residual[current][prev] += pathFlow;
            current = prev;
        }

        maxFlow += pathFlow;
    }

    // Find vertices reachable in the final residual graph.
    const reachable = new Set([source]);
    const queue = [source];

    while (queue.length) {
        const current = queue.shift();

        for (let next = 1; next <= 4; next++) {
            if (!reachable.has(next) && residual[current][next] > 0) {
                reachable.add(next);
                queue.push(next);
            }
        }
    }

    for (let u = 1; u <= 4; u++) {
        for (let v = 1; v <= 4; v++) {
            if (reachable.has(u) && !reachable.has(v) &&
                capacity[u][v] > 0) {
                cutEdges.push(`${u} → ${v}`);
            }
        }
    }

    return { maxFlow, cutEdges };
}

document.getElementById("calculate-flow").addEventListener("click", () => {
    const source = Number(document.getElementById("flow-source").value);
    const destination = Number(
        document.getElementById("flow-destination").value
    );

    if (source === destination) {
        showToast("Choose different source and destination.");
        return;
    }

    const result = calculateMaximumFlow(source, destination);

    document.getElementById("flow-value").innerHTML =
        `${result.maxFlow} <span>MW</span>`;

    document.getElementById("flow-message").textContent =
        `Maximum flow from ${stations[source]} to ${stations[destination]} using the current demo network.`;

    const cutContainer = document.getElementById("cut-lines");

    cutContainer.innerHTML = result.cutEdges.length
        ? result.cutEdges.map(edge =>
            `<span class="chip">${edge}</span>`
          ).join("")
        : '<span class="chip">No cut edges</span>';

    showToast("Maximum flow analysis completed!");
});


// ==========================================
// MST HIGHLIGHT
// ==========================================

document.getElementById("show-mst").addEventListener("click", () => {
    const selected = [1, 2, 4];

    if (selected.some(id => disabledLines.has(id))) {
        document.getElementById("mst-message").textContent =
            "A selected MST connection is switched OFF. Restore all lines to view the sample MST.";
        showToast("Restore the network first.");
        return;
    }

    document.querySelectorAll(".grid-line").forEach(line => {
        const id = Number(line.dataset.line);
        line.style.opacity = selected.includes(id) ? "1" : ".12";
    });

    document.getElementById("mst-message").textContent =
        "Highlighted connections: Line 1, Line 2 and Line 4. Sample total cost: 350.";

    showToast("Sample MST connections highlighted.");
});


// ==========================================
// FAULT SIMULATION
// ==========================================

document.getElementById("simulate-fault").addEventListener("click", () => {
    const id = Number(document.getElementById("fault-line").value);

    disabledLines.add(id);
    updateNetworkDisplay();

    const output = document.getElementById("fault-output");

    output.classList.add("failed-output");

    output.innerHTML = `
        <span class="table-status">● Line ${id} failed</span>
        <p>Transmission connection ${stations[lines[id].from]} → ${stations[lines[id].to]} is now unavailable in this demo simulation.</p>
        <p>Active transmission lines: ${4 - disabledLines.size} / 4</p>
        <p>Use Restore Network to return to the original sample network.</p>
    `;

    showToast(`Fault simulated on Line ${id}.`);
});


// ==========================================
// RESTORE NETWORK
// ==========================================

document.getElementById("restore-grid").addEventListener("click", () => {
    disabledLines.clear();
    updateNetworkDisplay();

    const output = document.getElementById("fault-output");

    output.classList.remove("failed-output");

    output.innerHTML = `
        <span class="table-status">● Network stable</span>
        <p>All four transmission lines have been restored in the demo simulation.</p>
    `;

    document.querySelectorAll(".grid-line").forEach(line => {
        line.style.opacity = "";
    });

    document.getElementById("mst-message").textContent =
        "Press the button to highlight the selected connections.";

    showToast("Network restored successfully!");
});


// ==========================================
// INITIALIZATION
// ==========================================

updateNetworkDisplay();

console.log("GridGuard interactive frontend loaded successfully!");
